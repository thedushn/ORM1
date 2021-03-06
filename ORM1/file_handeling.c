//
// Created by dushn on 14.12.17..
//


#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include "file_handeling.h"
#define BUFFER_SIZE 1400
#define BUFFER_SIZE2 1404

pthread_cond_t cond =  PTHREAD_COND_INITIALIZER;
pthread_mutex_t m =PTHREAD_MUTEX_INITIALIZER;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void server_prog(char *argv1, char *argv2, char *argv3){

    int num_connections=10;
    pthread_t t_main[num_connections];

    FILE *fp;
    int num_pthreads=atoi(argv2);

    pthread_attr_t attr;
    char buffer[BUFFER_SIZE];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    struct name_s name_s1[num_connections];







    char *filename=argv3;
    fp=fopen(filename,"r");
    if(fp==NULL){


        printf("could not open file \n");

    }
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    sprintf(buffer,"%li",file_size);
    fclose(fp);







    int sockfd=0;

    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, argv1, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        // return 1;
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }


        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }


        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

  //  while(i !=10)  {
    for(int i=0 ;i<10;i++){


        if(i>0){

            pthread_cond_wait (&cond, &m);

        }
        {  // main accept() loop
            printf("main accept loop\n");
            sin_size = sizeof their_addr;
            pthread_mutex_trylock(&m);
            name_s1[i].socket = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
            if (name_s1[i].socket == -1) {
                perror("accept");

            }
        }
        pthread_mutex_unlock(&m);
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        name_s1[i].thread_num=(uint16_t)num_pthreads;
        strcpy(name_s1[i].filename,filename);


        //sending the filename
        send_filename(&name_s1[i]);


        //using the server socket to make other sockets because we closed the other socket
        name_s1[i].socket=sockfd;

       int ret= pthread_create(&t_main[i],NULL,new_connection,&name_s1[i]);

        if (ret!=0){

            perror("error creating threads");
            exit(1);
        }






    }

    for(int i =0;i<num_connections;i++){

        pthread_join(t_main[i],NULL);
    }







    close(sockfd);
}
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void * new_connection(void *data_temp){

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    char s[INET6_ADDRSTRLEN];
    struct name_s name_s1;
    name_s1=  *((struct name_s *)data_temp);

    pthread_t  t[name_s1.thread_num];
    socklen_t sin_size;
    struct sockaddr_storage their_addr;

    struct data_s data_s1[name_s1.thread_num];


    FILE *fp;

    fp=fopen(name_s1.filename,"rb");
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);


    fclose(fp);



    /// u zavisnosti od broja konekcija delimo file na toliko delova




    int numb_bytes=(int)((file_size/name_s1.thread_num));

    for(int i=0;i<name_s1.thread_num;i++){
        memset(&data_s1[i],0,sizeof(struct data_s));
        data_s1[i].file_position_b=numb_bytes*i;
        data_s1[i].file_position_e=numb_bytes*(i+1);
    }
    if(data_s1[name_s1.thread_num-1].file_position_e!=file_size){
        data_s1[name_s1.thread_num-1].file_position_e=(int)file_size;
    }
    for(int i=0;i<name_s1.thread_num;i++){

        strcpy(data_s1[i].filename,name_s1.filename);
        data_s1[i].pack_number=i;

        {  // main accept() loop

            sin_size = sizeof their_addr;
            pthread_mutex_trylock(&m);

            data_s1[i].socket = accept(name_s1.socket, (struct sockaddr *) &their_addr, &sin_size);
            if (data_s1[i].socket == -1) {
                perror("accept");

            }
            pthread_mutex_unlock(&m);

        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);


        pthread_create(&t[i],NULL,new_file,&data_s1[i]);



    }

    void *status;
    pthread_attr_destroy(&attr);
    pthread_cond_signal(&cond);

    for(int i=0;i<name_s1.thread_num;i++){


        int rc= pthread_join(t[i],&status);

        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

    }

    for(int i=0;i<name_s1.thread_num;i++){


        close( data_s1[i].socket);

    }




    return  0;

}
void * new_file(void *data_temp){

        char buffer[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        char buffer3[BUFFER_SIZE2];
        char write_b[4];
        FILE *fp;
        ssize_t ret_1;
        struct data_s data_s1;
        data_s1=  *((struct data_s *)data_temp);
        int socket=data_s1.socket;
        unsigned int koliko_bytes=0;
        int koliko_treba;

        size_t read_temp=0;




        fp=fopen(data_s1.filename,"rb");

        if (fp == NULL) {
            printf("open failed, errno = %d\n", errno);
            exit(1);
        }
        fseek(fp, data_s1.file_position_b,SEEK_CUR );

    if(data_s1.file_position_b!=0){
       koliko_treba=data_s1.file_position_e-data_s1.file_position_b;


    }
    else{

        koliko_treba=data_s1.file_position_e;

    }






    sprintf(buffer," %d  %d %d %d" ,koliko_treba,data_s1.file_position_b,data_s1.file_position_e,data_s1.pack_number+1);
    ssize_t ret= send(socket,buffer,BUFFER_SIZE,0);
    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {


            ret = send(socket, buffer, velicina, 0);
            velicina -= ret;

            if(ret==0){

                printf("socket closed\n");

                fclose(fp);
                pthread_exit(NULL);

            }
            if (ret < 0) {

                printf("error sending data\n %d", (int) ret);
                printf("server nije ni poslao podatke clientu\n");

                fclose(fp);
                pthread_exit(NULL);
            }
        }
    }


    memset(buffer2,0,BUFFER_SIZE);
    ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);

    if(ret_1<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret_1;
        while (velicina > 0 || velicina < 0) {


            ret_1 = recv(socket, buffer2, velicina, 0);
            printf("Return value %d\n",(int)ret_1);
            velicina -= ret_1;

            if(ret_1==0){

                printf("socket closed\n");

                fclose(fp);

                pthread_exit(NULL);

            }
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);

                fclose(fp);
                pthread_exit(NULL);

            }
        }
        printf("Return value %d\n",(int)ret_1);

    }

    if (strcmp(buffer2, "stiglo sve") != 0) {

        printf("data didnt get received correctly   \n");

        fclose(fp);
        pthread_exit(NULL);
    }

    while(1){


        memset(buffer,0,BUFFER_SIZE);
        memset(buffer2,0,BUFFER_SIZE);


        if(koliko_treba<=0){

            memset(buffer3,0,BUFFER_SIZE2);
            memcpy(buffer3,"1111",4);
            strcat(buffer3,"end of file");
            ret_1=send(socket,buffer3,BUFFER_SIZE2,0);
            koliko_bytes+=ret_1;
            if(ret_1<0){
                printf("Error sending num_packets!\n\t");

                fclose(fp);
                pthread_exit(NULL);
            }

            if(ret_1<BUFFER_SIZE2) {

                size_t velicina = BUFFER_SIZE2;
                while (velicina > 0) {
                    velicina -= ret_1;
                    ret_1 = send(socket, buffer3, velicina, 0);
                    koliko_bytes+=ret_1;
                    if (ret_1 < 0) {

                        printf("error receving data\n %d", (int) ret_1);

                        fclose(fp);
                        pthread_exit(NULL);
                    }
                }

            }


            ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);

                fclose(fp);
                pthread_exit(NULL);
            }
            if(ret_1<BUFFER_SIZE) {
                size_t velicina = BUFFER_SIZE;
                velicina -= ret_1;
                while (velicina > 0 || velicina < 0) {
                    printf("Buffer [%s]\n", buffer);

                    ret_1 = recv(socket, buffer, velicina, 0);
                    velicina -= ret_1;
                    koliko_bytes += ret_1;
                    if(ret_1==0){

                        printf("socket closed\n");

                        fclose(fp);
                        pthread_exit(NULL);
                    }
                    if (ret_1 < 0) {

                        printf("error receving data\n %d", (int) ret_1);

                        fclose(fp);
                        pthread_exit(NULL);
                    }
                }

            }
            if (strcmp(buffer, "stiglo sve") == 0) {


                fclose(fp);
                break;
            }


        }

        size_t nread;

        nread=fread(buffer,1,BUFFER_SIZE,fp);

        read_temp+=nread;
       if(nread>koliko_treba || nread==koliko_treba){


            memcpy(buffer2,buffer,(size_t)koliko_treba);

           nread=(size_t)koliko_treba;
           koliko_treba-=koliko_treba;

        }
       else{


            koliko_treba-=nread;
            memcpy(buffer2,buffer,nread);
        }




        sprintf(write_b,"%d",(__uint16_t)nread);
        memset(buffer3,0,BUFFER_SIZE2);
        memcpy(buffer3+0, write_b,sizeof(write_b));
        memcpy(buffer3+4,buffer2,BUFFER_SIZE);



        ret_1=send(socket,buffer3,BUFFER_SIZE2,0);


        if(ret_1<0){
            printf("Error sending num_packets!\n\t");

            fclose(fp);
            pthread_exit(NULL);
        }
        if(ret_1==0){
            printf("Error sending num_packets!\n\t");
            printf("socket closed\n");

            fclose(fp);
            pthread_exit(NULL);
        }
        koliko_bytes+=ret_1;

        if(ret_1<BUFFER_SIZE2){
            size_t velicina=BUFFER_SIZE2;
            velicina-=ret_1;
            while (velicina > 0 || velicina < 0) {
                printf("Buffer2 [%s]\n", buffer2);

                ret_1 = send(socket, buffer3, velicina, 0);
                velicina -= ret_1;
                koliko_bytes += ret_1;

                if (ret_1 < 0) {

                    printf("error sending data\n %d", (int) ret_1);

                    fclose(fp);
                    pthread_exit(NULL);
                }
                if(ret_1==0){
                    printf("Error sending num_packets!\n\t");
                    printf("socket closed\n");

                    fclose(fp);
                    pthread_exit(NULL);
                }
            }
        }





        ///primamo poruku da je klient primio poruku

        memset(buffer,0,BUFFER_SIZE);
        ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
        if (ret_1 < 0) {

            printf("error receving data\n %d", (int) ret_1);

            fclose(fp);
            pthread_exit(NULL);
        }
        if(ret_1<BUFFER_SIZE) {
            size_t velicina = BUFFER_SIZE;
            velicina -= ret_1;
            while (velicina > 0 || velicina < 0) {
                printf("Buffer2 [%s]\n", buffer);

                ret_1 = recv(socket, buffer, velicina, 0);
                velicina -= ret_1;
                koliko_bytes += ret_1;
                if(ret_1==0){

                    printf("socket closed\n");

                    fclose(fp);
                    pthread_exit(NULL);
                }
                if (ret_1 < 0) {

                    printf("error receving data\n %d", (int) ret_1);

                    fclose(fp);
                    pthread_exit(NULL);
                }
            }

        }
        /// ako nije dobro primljeno
        if(strcmp(buffer, "stiglo sve") == 0){

            char buffer_temp [BUFFER_SIZE];
            memset(buffer_temp,0,BUFFER_SIZE);
            memcpy(buffer_temp,buffer3+4,BUFFER_SIZE);


        }
        else{


            fclose(fp);

           pthread_exit(NULL);
        }




    }




    close(socket);

        return 0;
}





void * send_filename(void *socket_tmp){


    ssize_t ret=0;
    int socket=0;
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char numbers[4];
    struct name_s name_s1=*(struct name_s *) socket_tmp;



    socket= name_s1.socket;
    sprintf(numbers,"%" SCNu16 "",name_s1.thread_num);
    memcpy(buffer,numbers,sizeof(numbers));
    memcpy(buffer+sizeof(numbers),name_s1.filename,sizeof(name_s1.filename));



     ret= send(socket,buffer,BUFFER_SIZE,0);
    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {
            //  printf("Buffer2 [%s]\n", buffer_2);

            ret = send(socket, buffer, velicina, 0);
            velicina -= ret;
            //  koliko_bytes += ret;
            if (ret < 0) {

                printf("error sending data\n %d", (int) ret);
                exit(1);
            }
        }
    }

    memset(buffer2,0,BUFFER_SIZE);
    ssize_t  ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
    if (ret_1 < 0) {

        printf("error receving data\n %d", (int) ret_1);
        exit(1);
    }
    if(ret_1==0){

        printf("error receving data\n %d", (int) ret_1);
        exit(1);

    }
    if(ret_1<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret_1;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer2 [%s]\n", buffer2);

            ret_1 = recv(socket, buffer2, velicina, 0);
            velicina -= ret_1;

            if (ret_1 < 0) {

                printf("error receing data\n %d", (int) ret_1);
                exit(1);
            }
            if(ret_1==0){

                printf("error receving data\n %d", (int) ret_1);
                exit(1);

            }
        }

    }

    if (strcmp(buffer2, "stiglo sve") != 0) {



        exit(1);
    }



      close(socket);



    return 0;
};


