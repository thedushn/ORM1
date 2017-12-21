#include <stdio.h>
#include"stdio.h"
#include <inttypes.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <pthread.h>
#include"pthread.h"
#include"sys/socket.h"
#include "file_handeling.h"
#include <stdlib.h>

#include <arpa/inet.h>
#include <unistd.h>

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
int main(int argc, char *argv[]) {


    pthread_t t, t2;
    int sockfd=0;
    int new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv, ret, ret2;
    char buffer[BUFFER_SIZE];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if (argc < 2) {

        printf("no port provided");
        exit(1);
    }
    // uint16_t portnum=(uint16_t)atoi(argv[1]);

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
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


    {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");

        }
    }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *) &their_addr),
              s, sizeof s);
    printf("server: got connection from %s\n", s);



    FILE *fp;
    char *filename="test.txt";
    fp=fopen(filename,"r");
    long	file_size;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, 0);
    sprintf(buffer,"%li",file_size);
    fclose(fp);
    /// u zavisnosti od broja konekcija delimo file na toliko delova  za sada 4
    printf("file_size %li \n",file_size);
    int numb_packets;
    if(((int)file_size/BUFFER_SIZE)>BUFFER_SIZE){

        numb_packets=(((int)file_size/BUFFER_SIZE);
    }
    else
    {
        numb_packets=1;
    }


   // recv_files(&new_fd);


     struct data_s *data_s1=calloc(1,sizeof(struct data_s));


 strcpy(data_s1->filename,filename);
 data_s1->socket=new_fd;
    data_s1->file_size=(int)file_size;
    data_s1->numb_packets=numb_packets;
    data_s1->pack_number=1;
ret2= pthread_create(&t2,NULL,send_files,&new_fd);
 if(ret2!=0){
     // if( pthread_create(&t2, NULL, slanje, &info)){
     printf("ERROR: Return Code from pthread_create() is %d\n",ret2);
     exit(1);

 }
 int  new_fd1=0;
 {  // main accept() loop
     sin_size = sizeof their_addr;
    new_fd1 = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
     if (new_fd == -1) {
         perror("accept");

     }
 }
 inet_ntop(their_addr.ss_family,
           get_in_addr((struct sockaddr *) &their_addr),
           s, sizeof s);
 printf("server: got connection from %s\n", s);
//  send_files(&new_fd);
ret2=  pthread_create(&t,NULL,send_files,&new_fd1);
 if(ret2!=0){
     // if( pthread_create(&t2, NULL, slanje, &info)){
     printf("ERROR: Return Code from pthread_create() is %d\n",ret2);
     exit(1);

 }
 pthread_join(t,NULL);
 pthread_join(t2,NULL);


 if (pthread_mutex_init(&m, NULL) != 0) {
     printf("\n mutex init failed\n");
     return 1;
 }




//  splitFile(filename,  /*3000000 */((size_t)file_size/2)+1 );
  //  reassemble(4,filename);
    clock_t begin = clock();
   // merge();

/* here, do your time-consuming job */
   /* struct data_s *data_s1=calloc(1,sizeof(struct data_s));
    strcpy(data_s1->filename,filename);
    data_s1->socket=new_fd;
    ret2=  pthread_create(&t,NULL,file_handeling,data_s1);
    if(ret2!=0){
        // if( pthread_create(&t2, NULL, slanje, &info)){
        printf("ERROR: Return Code from pthread_create() is %d\n",ret2);
        exit(1);

    }*/
    pthread_join(t,NULL);
 //   file_handeling(new_fd);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("TIME spent %f \n",time_spent);




    printf("sve proslo kako treba\n");
    pthread_mutex_destroy(&m);
    printf("file_size %li \n",file_size);

    free(data_s1);
    //close(sockfd);
return 0;
}