//
// Created by dushn on 14.12.17..
//

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>

#include "file_handeling.h"

#define BUFFER_SIZE 1400
void *recv_files(void *socket_tmp){
    int socket= *(int *) socket_tmp;
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    printf("Usli smo u Recv_files\n");
    ssize_t ret_1=recv(socket,buffer,BUFFER_SIZE,0);
    if (ret_1 < 0) {

        printf("error receing data\n %d", (int) ret_1);
        exit(1);
    }
    if(ret_1<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret_1;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer [%s]\n", buffer);

            ret_1 = recv(socket, buffer, velicina, 0);
            velicina -= ret_1;
            ///   koliko_bytes += ret_1;
            if (ret_1 < 0) {

                printf("error receing data\n %d", (int) ret_1);
                exit(1);
            }
        }
    }
    char buffer2[BUFFER_SIZE];
    memset(buffer2,0,BUFFER_SIZE);
    strcpy(buffer2,"stiglo sve");
    ssize_t ret=send(socket,buffer2,BUFFER_SIZE,0);

    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer2 [%s]\n", buffer2);

            ret = send(socket, buffer2, velicina, 0);
            velicina -= ret;
            //koliko_bytes += ret;
            if (ret < 0) {

                printf("error receing data\n %d", (int) ret);
                exit(1);
            }
        }
    }

    printf("Buffer %s\n",buffer);
    printf("Socket number %d\n",socket);
   // close(socket);
   // printf("closed socket\n");
    return 0;


}
void * send_files(void *socket_tmp){

    printf("usli smo u send_files\n");
    int socket= *(int *) socket_tmp;
        char buffer[BUFFER_SIZE];

       strcpy(buffer,"hello server");
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
void create_file(int socket){

    FILE *fp;
    char buffer[BUFFER_SIZE];
    char buffer_2[BUFFER_SIZE];
    char buffer_3[BUFFER_SIZE];
    ssize_t ret=0;
    int file_size=0;
    char *filename_b=malloc(64);
    char *filename_e=malloc(64);
   static int koliko_bytes=0;
    char *filename=malloc(64);
    ret = recv(socket,buffer,BUFFER_SIZE, 0);
  //  koliko_bytes+=ret;
    printf("Return value : [%d]",(int)ret);
    if(ret<0){

        printf("error receing data\n %d",(int)ret);
    }
    if(ret<BUFFER_SIZE){
        size_t velicina=BUFFER_SIZE;
        while(velicina>0){
            printf("[%s]\n",buffer);
            velicina-=ret;
            ret=recv(socket,buffer,velicina, 0);
            koliko_bytes+=ret;
            if(ret<0){

                printf("error receing data\n %d",(int)ret);
            }
            printf("Return value %d",(int)ret);
        }
    }
    memset(buffer_2,0,BUFFER_SIZE);
    strcpy(buffer_2,"stiglo sve");
    ret=send(socket,buffer_2,BUFFER_SIZE,0);

    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer2 [%s]\n", buffer_2);

            ret = send(socket, buffer_2, velicina, 0);
            velicina -= ret;
          //  koliko_bytes += ret;
            if (ret < 0) {

                printf("error receing data\n %d", (int) ret);
                exit(1);
            }
        }
    }



    sscanf(buffer,"%s %d %s %s",filename,&file_size,filename_b,filename_e);
    printf("File size %d",file_size);
    //ret = recv(socket,buffer,BUFFER_SIZE, 0);
   // printf("Return value : [%d]",(int)ret);
    if(ret<0){

        printf("error receing data\n %d",(int)ret);
    }
    if(ret<BUFFER_SIZE){
        size_t velicina=BUFFER_SIZE;
        while(velicina>0){
            printf("[%s]\n",buffer);
            velicina-=ret;
            ret=recv(socket,buffer,velicina, 0);
            koliko_bytes+=ret;
            if(ret<0){

                printf("error receing data\n %d",(int)ret);
            }
            printf("Return value %d",(int)ret);
        }
    }

    if(file_size>0){
        memset(buffer_3,0,BUFFER_SIZE);
        strcat(buffer_3,filename);
        strcat(buffer_3,".");
        strcat(buffer_3,filename_b);
        strcat(buffer_3,"-");
        strcat(buffer_3,filename_e);
        fp =fopen(buffer_3,"w+");
        if (fp == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        while(1){
            memset(buffer,0,BUFFER_SIZE);
            
            ret = recv(socket,buffer,BUFFER_SIZE, 0);
            printf("Return value %d",(int)ret);
            koliko_bytes+=ret;
            if(ret<BUFFER_SIZE){
                size_t velicina=BUFFER_SIZE;
                velicina-=ret;
                while(velicina>0 || velicina <0){
                    printf("Buffer [%s]\n",buffer);

                    ret=recv(socket,buffer,velicina, 0);
                    velicina-=ret;
                    koliko_bytes+=ret;
                    if(ret<0){

                        printf("error receing data\n %d",(int)ret);
                        exit(1);
                    }
                    if(ret==0){

                        printf("socket closed\n");
                        break;
                    }
                    printf("Return value %d",(int)ret);
                }
            }
            int t=0;
            printf("buffer:  [%s]\n",buffer);
            memset(buffer_2,0,BUFFER_SIZE);
            strcpy(buffer_2,"stiglo sve");
            ret=send(socket,buffer_2,BUFFER_SIZE,0);

            if(ret<BUFFER_SIZE) {
                size_t velicina = BUFFER_SIZE;
                velicina -= ret;
                while (velicina > 0 || velicina < 0) {
                    printf("Buffer2 [%s]\n", buffer_2);

                    ret = send(socket, buffer_2, velicina, 0);
                    velicina -= ret;
                    koliko_bytes += ret;
                    if (ret < 0) {

                        printf("error receing data\n %d", (int) ret);
                        exit(1);
                    }
                }
            }
            if( (t=strcmp(buffer,"end of file"))==0) {
                printf("dosli smo do kraja file\n");
                fclose(fp);
                break;
            }

          /*  if(buffer[0]==0){

                printf("kraj file\n");
                fclose(fp);
               // exit(1);.
                break;
            }*/
            ssize_t j= strlen(buffer);
            printf("string len of buffer %d  Buffer[%s]",(int) j,buffer);
            //fwrite(buffer,1,strlen(buffer),fp);
            fwrite(buffer,1,BUFFER_SIZE,fp);
        }

    }



    //fclose(fp);
    printf("Buffer [%s]\n",buffer);
    printf("koliko bytes primljeno %d\n",koliko_bytes);
/*    memset(buffer,0,BUFFER_SIZE);
    strcpy(buffer,"stiglo sve");
    ret=send(socket,buffer,BUFFER_SIZE,0);
    if (ret < 0) {

        printf("error sending data\n %d", (int) ret);
        exit(1);
    }
    if(ret<BUFFER_SIZE) {
        size_t velicina = BUFFER_SIZE;
        velicina -= ret;
        while (velicina > 0 || velicina < 0) {
            printf("Buffer [%s]\n", buffer);

            ret = send(socket, buffer, velicina, 0);
            velicina -= ret;
            koliko_bytes += ret;

        }
    }*/
    free(filename);
    free(filename_e);
    free(filename_b);

};