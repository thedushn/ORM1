#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <zconf.h>
#include"pthread.h"
#include "file_handeling.h"

#define  BUFFER_SIZE 1400

int  conection(char * argv1, char * argv2){


    struct addrinfo hints, *servinfo, *p;
    int rv;
    int newsockfd=0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    //192.168.122.70 127.0.0.1
    if ((rv = getaddrinfo(argv2,argv1, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

// loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((newsockfd = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(newsockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(newsockfd);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no connection
        free(servinfo);
        fprintf(stderr, "failed to connect\n");
        exit(2);
    }



    if(newsockfd<0){
        printf("Error creating socket!\n");
        exit(1);
    }
    printf("Socket created \n");



    printf("Connected to the server...\n");
    free(servinfo);
    return newsockfd;
};
int main(int argc, char *argv[]) {



    int socket_file=0;



   /* if(argc<3){

        printf("port not provided \n");
        printf("ip_address not provided \n");
        exit(1);
    }

    if(argv[1]==NULL){
        printf("argv failed %s",argv[1]);
        exit(1);

    }
    if(argv[2]==NULL){
        printf("argv failed %s",argv[2]);
        exit(1);

    }*/
    clock_t begin = clock();


/* here, do your time-consuming job */





   socket_file= conection(argv[1], argv[2]);
    if (socket_file< 0) {

        printf("socket failed%d \n", socket_file);
        exit(1);
    }
    char *name=(char *)calloc(64,sizeof(char));
    int num_threads=0;
    get_filename(&socket_file,name,&num_threads );
  //  pthread_t  t[num_threads];
    pthread_t *t ;
    t=(pthread_t *)malloc(num_threads * sizeof(pthread_t ));
    int socket_num[num_threads];
    for(int i=0;i<num_threads;i++) {


        socket_num[i] = conection(argv[1],argv[2]);
        if (socket_num[i] < 0) {

            printf("socket failed%d \n", socket_num[i]);
            exit(1);
        }
    }

    for(int i=0;i<num_threads;i++){

            printf("Socket %d\n",socket_num[i]);
        int ret2=   pthread_create(&t[i],NULL,create_file,&socket_num[i]);
        if(ret2!=0){

            printf("ERROR: Return Code from pthread_create() is %d\n",ret2);
            exit(1);

        }

    }

    void *status;

    for(int i=0;i<num_threads;i++){


        int rc= pthread_join(t[i],&status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %d having a status of %ld \n",i,(long)status);
    }




   // printf("ulazimo u merge\n");
    merge(num_threads,name);
  //  get_filename(&num_threads);
    printf("name %s\n",name);
    printf("sve proslo kako treba\n");
        for(int i=0;i<num_threads;i++){

            close(socket_num[i]);
        }
        close(socket_file);
    clock_t end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("TIME spent %f \n",time_spent);
    free(name);
  free(t);


    return 0;
}