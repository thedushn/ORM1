

#include "file_handeling.h"

#include <stdio.h>
#include <errno.h>
#include <wait.h>
#include <bits/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "pthread.h"
#include <arpa/inet.h>
#include <math.h>
#define  BUFFER_SIZE 1400
pthread_mutex_t m;
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void conection(int * socket_new,char *port){


}
int main(int argc, char *argv[]) {

    int num_pthreads=1;
    pthread_attr_t attr;
    pthread_t t[num_pthreads], t2;
    char buffer[BUFFER_SIZE];
    int ret2=0;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


   if (argc < 2) {

        printf("no port provided");
        exit(1);
    }
    // uint16_t portnum=(uint16_t)atoi(argv[1]);



    int sockfd=0;
    int new_fd;  // listen on sock_fd, new connection on new_fd
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

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
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






    FILE *fp;
    char *filename="rc.jpg";
    fp=fopen(filename,"r");
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    //file_size=15555;
    fseek(fp, 0, 0);
    sprintf(buffer,"%li",file_size);
    fclose(fp);
    /// u zavisnosti od broja konekcija delimo file na toliko delova  za sada 4
    printf("file_size %li \n",file_size);
    float numb_packets=0;

    if((int)file_size>BUFFER_SIZE){

        numb_packets=(((float)file_size/BUFFER_SIZE));
    numb_packets=   ceilf(numb_packets);
    }
    else
    {
        numb_packets=1;
    }

    int numb_bytes=(int)((file_size/num_pthreads));

      //  numb_bytes=ceilf(numb_bytes);

    struct data_s data_s1[num_pthreads];

    for(int i=0;i<num_pthreads;i++){
        memset(&data_s1[i],0,sizeof(struct data_s));
        data_s1[i].file_position_b=numb_bytes*i;
        data_s1[i].file_position_e=numb_bytes*(i+1);
    }
    if(data_s1[num_pthreads-1].file_position_e!=file_size){
        data_s1[num_pthreads-1].file_position_e=(int)file_size;
    }
    for(int i=0;i<num_pthreads;i++){
       //memset(&data_s1[i],0,sizeof(struct data_s));
        strcpy(data_s1[i].filename,filename);
        data_s1[i].file_size=(int)file_size;
       // data_s1[i].file_position_b=(int)numb_bytes*i;
       // data_s1[i].file_position_e=(int)numb_bytes*(i+1);
        data_s1[i].numb_packets=(int)numb_packets*(i+1);
        data_s1[i].pack_number=i;
      //  new_file(&data_s1[i]);
        {  // main accept() loop
            sin_size = sizeof their_addr;
            data_s1[i].socket = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
            if (data_s1[i].socket == -1) {
                perror("accept");

            }
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

    for(int i=0;i<num_pthreads;i++){


       int rc= pthread_join(t[i],&status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %d having a status of %ld\n",i,(long)status);
    }






 if (pthread_mutex_init(&m, NULL) != 0) {
     printf("\n mutex init failed\n");
     return 1;
 }



    clock_t begin = clock();


/* here, do your time-consuming job */



    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("TIME spent %f \n",time_spent);




    printf("sve proslo kako treba\n");
    pthread_mutex_destroy(&m);
  //  printf("file_size %li \n",file_size);
    test();
  //  free(data_s1);
 //   close(sockfd);

return 0;
}