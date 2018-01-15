

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
    server_prog(argv[1],argv[2]);


    clock_t begin = clock();


/* here, do your time-consuming job */



    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("TIME spent %f \n",time_spent);




    printf("sve proslo kako treba\n");

  //  printf("file_size %li \n",file_size);
 //  test();
  //  free(data_s1);


return 0;
}