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
#include "file_handeling.h"
#define BUFFER_SIZE 1400
#define BUFFER_SIZE2 1404

pthread_mutex_t m =PTHREAD_MUTEX_INITIALIZER;
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

    fp=fopen(name_s1.filename,"r");
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    //file_size=15555;    fseek(fp, 0, 0);

    fclose(fp);



    /// u zavisnosti od broja konekcija delimo file na toliko delova  za sada 4
    printf("file_size %li \n",file_size);
    float numb_packets=0;


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
        //memset(&data_s1[i],0,sizeof(struct data_s));
        strcpy(data_s1[i].filename,name_s1.filename);
        data_s1[i].file_size=(int)file_size;
        // data_s1[i].file_position_b=(int)numb_bytes*i;
        // data_s1[i].file_position_e=(int)numb_bytes*(i+1);
        data_s1[i].numb_packets=(int)numb_packets*(i+1);
        data_s1[i].pack_number=i;
        //  new_file(&data_s1[i]);
        {  // main accept() loop
            sin_size = sizeof their_addr;
            pthread_mutex_lock(&m);
            data_s1[i].socket = accept(name_s1.socket, (struct sockaddr *) &their_addr, &sin_size);
            if (data_s1[i].socket == -1) {
                perror("accept");

            }
            pthread_mutex_unlock(&m);
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_create(&t[i],NULL,new_file,&data_s1[i]);
        // new_file(&data_s1[i]);


    }
    void *status;
    pthread_attr_destroy(&attr);

    for(int i=0;i<name_s1.thread_num;i++){


        int rc= pthread_join(t[i],&status);
        printf("thread joined %d\n",i);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %d having a status of %ld\n",i,(long)status);
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
      ///  char *filename=malloc(64);
        struct data_s data_s1;
        data_s1=  *((struct data_s *)data_temp);
        int socket=data_s1.socket;
        static int koliko_bytes;
        int koliko_treba;
        int koliko_treba_1;
        size_t read_temp=0;
      ///  strcpy(filename,data_s1.filename);



        fp=fopen(data_s1.filename,"rb");

        if (fp == NULL/* || fp_temp==NULL*/) {
            printf("open failed, errno = %d\n", errno);
            exit(1);
        }
        fseek(fp, data_s1.file_position_b,SEEK_CUR );

    if(data_s1.file_position_b!=0){
       koliko_treba=data_s1.file_position_e-data_s1.file_position_b;
        koliko_treba_1=data_s1.file_position_e-data_s1.file_position_b;
       //koliko_treba=data_s1.file_position_e-data_s1.file_position_b+1;
    }
    else{

        koliko_treba=data_s1.file_position_e;
       koliko_treba_1= data_s1.file_position_e;
    }






    sprintf(buffer,"%s %d  %d %d %d" ,data_s1.filename,koliko_treba,data_s1.file_position_b,data_s1.file_position_e,data_s1.pack_number+1);
    printf("Buffer %s \n",buffer);
    ssize_t ret= send(socket,buffer,BUFFER_SIZE,0);
    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {
            //  printf("Buffer2 [%s]\n", buffer_2);

            ret = send(socket, buffer, velicina, 0);
            velicina -= ret;
            //  koliko_bytes += ret;
            if(ret==0){

                printf("socket closed\n");
               // free(filename);
                fclose(fp);
                pthread_exit(NULL);

            }
            if (ret < 0) {

                printf("error sending data\n %d", (int) ret);
                printf("server nije ni poslao podatke clientu\n");
             //   free(filename);
                fclose(fp);
                pthread_exit(NULL);
            }
        }
    }
  //  printf("Return value %d\n",(int)ret);

    memset(buffer2,0,BUFFER_SIZE);
    ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);

    if(ret_1<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret_1;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer2 [%s]\n", buffer2);

            ret_1 = recv(socket, buffer2, velicina, 0);
            printf("Return value %d\n",(int)ret_1);
            velicina -= ret_1;
            // koliko_bytes += ret_1;

            if(ret_1==0){

                printf("socket closed\n");
             //   free(filename);
                fclose(fp);

                pthread_exit(NULL);

            }
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);
               // free(filename);
                fclose(fp);
                pthread_exit(NULL);
                exit(1);
            }
        }
        printf("Return value %d\n",(int)ret_1);

    }

    if (strcmp(buffer2, "stiglo sve") != 0) {

        printf("NOPE  \n");
      //  free(filename);
        fclose(fp);
        pthread_exit(NULL);
    }

    while(1){


        memset(buffer,0,BUFFER_SIZE);
        memset(buffer2,0,BUFFER_SIZE);


        if(koliko_treba<=0){
            printf("stigli do kraja file\n");
            memset(buffer3,0,BUFFER_SIZE2);
            memcpy(buffer3,"1111",4);
            strcat(buffer3,"end of file");
            ret_1=send(socket,buffer3,BUFFER_SIZE2,0);
            koliko_bytes+=ret_1;
            if(ret_1<0){
                printf("Error sending num_packets!\n\t");
              //  free(filename);
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
                    //    free(filename);
                        fclose(fp);
                        pthread_exit(NULL);
                    }
                }

            }

            printf("end of file  msg\n");
            ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);
               // free(filename);
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
                      //  free(filename);
                        fclose(fp);
                        pthread_exit(NULL);
                    }
                    if (ret_1 < 0) {

                        printf("error receving data\n %d", (int) ret_1);
                       // free(filename);
                        fclose(fp);
                        pthread_exit(NULL);
                    }
                }

            }
            if (strcmp(buffer, "stiglo sve") == 0) {

                printf("Zatvaramo file \n");
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

       // (size_t)(data_s1.file_position_e-data_s1.file_position_b)
        // printf("Strlen Buf %d strlen buff2 %d\n",(int)strlen(buffer),(int)strlen(buffer2));
      //  printf("Buffer2 [%s]\n",buffer2);


        sprintf(write_b,"%d",(__uint16_t)nread);
        memset(buffer3,0,BUFFER_SIZE2);
        memcpy(buffer3+0, write_b,sizeof(write_b));
        memcpy(buffer3+4,buffer2,BUFFER_SIZE);
                //sprintf(buffer3,"%" SCNu16 "",packet_size);
     //   printf("Strlen Buf3 %d \n",(int)strlen(buffer3));


        ret_1=send(socket,buffer3,BUFFER_SIZE2,0);

      //  printf("Return value %d\n",(int)ret_1);
        if(ret_1<0){
            printf("Error sending num_packets!\n\t");
         //   free(filename);
            fclose(fp);
            pthread_exit(NULL);
        }
        if(ret_1==0){
            printf("Error sending num_packets!\n\t");
            printf("socket closed\n");
          //  free(filename);
            fclose(fp);
            pthread_exit(NULL);
        }
        koliko_bytes+=ret_1;
//            if(ret_1<strlen(buffer2)){
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
                  //  free(filename);
                    fclose(fp);
                    pthread_exit(NULL);
                }
                if(ret_1==0){
                    printf("Error sending num_packets!\n\t");
                    printf("socket closed\n");
                  //  free(filename);
                    fclose(fp);
                    pthread_exit(NULL);
                }
            }
        }





        ///primamo poruku da je klient primio poruku
     //   memset(buffer2,0,BUFFER_SIZE);
        memset(buffer,0,BUFFER_SIZE);
        ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
        if (ret_1 < 0) {

            printf("error receving data\n %d", (int) ret_1);
          //  free(filename);
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
                 //   free(filename);
                    fclose(fp);
                    pthread_exit(NULL);
                }
                if (ret_1 < 0) {

                    printf("error receving data\n %d", (int) ret_1);
                   // free(filename);
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

        //  size_t write_temp= fwrite(buffer_temp,1,nread,fp_temp);

            //   printf("write [%d] \n",(int)write_temp);

        }
        else{

            printf("NOPE  \n");
            fclose(fp);

            exit(1);
        }
     /*   if (strcmp(buffer2, buffer) != 0) {

            printf("NOPE  \n");
            fclose(fp);
            exit(1);
        }*/



    }



    printf("koliko bytes poslato %d\n",koliko_bytes);
    printf("koliko treba %d\n",koliko_treba_1);
    printf("izasli smo iz thread\n");
    printf("koliko podataka smo iscitali %d\n",(int)read_temp);
   // close(fd);
  //  close(fp2);
  //  fclose(fp_temp);
        return 0;
}





void * send_filename(void *socket_tmp){

    printf("usli smo u send_filename\n");

    struct name_s name_s1=*(struct name_s *) socket_tmp;
    int socket= name_s1.socket;
    char buffer[BUFFER_SIZE];
    char numbers[4];
    sprintf(numbers,"%" SCNu16 "",name_s1.thread_num);
    memcpy(buffer,numbers,sizeof(numbers));
    memcpy(buffer+sizeof(numbers),name_s1.filename,sizeof(name_s1.filename));
//    strcpy(buffer,name_s1.filename);
    ssize_t ret= send(socket,buffer,BUFFER_SIZE,0);
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
    char buffer2[BUFFER_SIZE];
    memset(buffer2,0,BUFFER_SIZE);
    ssize_t  ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
    if (ret_1 < 0) {

        printf("error receing data\n %d", (int) ret_1);
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
            // koliko_bytes += ret_1;
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

        printf("NOPE  \n");
        // fclose(fp);
        exit(1);
    }


    printf("Socket number %d\n",socket);
    //  close(socket);
    // sleep(2);


    return 0;
};


