

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


void log_stats(char * filename){
    FILE *fp;
    int thread_numb=0;
    fp=fopen(filename,"rb");
    char buffer[1024];
    char name[64];
    float f=0;
    float f1=0;
    int i=0;
    if(fp==NULL){

        printf("cant open file\n");
        exit(1);
    }

    while (fgets(buffer, 1024, fp) != NULL){
            if(i==10){

                printf("name %s thread_num %d float_prosek %f\n",name,thread_numb,f1/i);
                i=0;
                f1=0;
                //break;
            }
        sscanf(buffer,"name %s num_threads %d ",name,&thread_numb);
        if(fgets(buffer, 1024, fp) != NULL){

            sscanf(buffer,"TIME spent %f ",&f);
            f1+=f;
            i++;
        }
    }

    printf("name %s thread_num %d float_prosek %f\n",name,thread_numb,f1/10);


    fclose(fp);

};

int main(int argc, char *argv[]) {

    if (argc < 2) {

        printf("no port provided\n");
        printf("argv: port, number of threads \n");
        exit(1);
    }
    if(argc <3 ){

        printf("number of pthreads not provided\n");
        printf("argv: port, number of threads \n");
        exit(1);
    }
    if(argc <4 ){

        printf("filename not provided\n");
        printf("argv: port, number of threads,filename \n");
        exit(1);
    }
    server_prog(argv[1],argv[2],argv[3]);
  /*  log_stats("log1.txt");
    log_stats("log2.txt");
    log_stats("log3.txt");*/




return 0;
}