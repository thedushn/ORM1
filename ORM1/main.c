

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

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


int main(int argc, char *argv[]) {


    pthread_t t_main[10];

    int num_connections=0;
    int num_pthreads=4;
    pthread_attr_t attr;
    char buffer[BUFFER_SIZE];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    struct name_s name_s1[num_connections];

   if (argc < 2) {

        printf("no port provided");
        exit(1);
    }
    // uint16_t portnum=(uint16_t)atoi(argv[1]);



    FILE *fp;
    char *filename="log.txt";
    fp=fopen(filename,"r");
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    //file_size=15555;    fseek(fp, 0, 0);
    sprintf(buffer,"%li",file_size);
    fclose(fp);




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

  while(num_connections <10)  {

        {  // main accept() loop
            sin_size = sizeof their_addr;
            name_s1[num_connections].socket = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
            if (name_s1[num_connections].socket == -1) {
                perror("accept");

            }
        }
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        name_s1[num_connections].thread_num=(uint16_t)num_pthreads;
        strcpy(name_s1[num_connections].filename,filename);


        //sending the filename
        send_filename(&name_s1[num_connections]);


        //using the server socket to make other sockets
        name_s1[num_connections].socket=sockfd;

        pthread_create(&t_main[num_connections],NULL,new_connection,&name_s1[num_connections]);


    pthread_join(t_main[num_connections],NULL);
      num_connections++;
    }












    clock_t begin = clock();


/* here, do your time-consuming job */



    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("TIME spent %f \n",time_spent);




    printf("sve proslo kako treba\n");

  //  printf("file_size %li \n",file_size);
 //  test();
  //  free(data_s1);
    close(sockfd);

return 0;
}