//
// Created by dushn on 14.12.17..
//

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <dirent.h>

#include "file_handeling.h"

#define BUFFER_SIZE 1400
static int myCompare (const void * a, const void * b)
{
    return strcmp (*(const char **) a, *(const char **) b);
}

void sort(const char *arr[], int n)
{
    qsort (arr, n, sizeof (const char *), myCompare);
}
void merge (const int broj_thread){



    DIR *dir;
    struct dirent *d_file;
    char cwd[1024];
    char *names[broj_thread];

    //char (*file_name_array)[64];
    getcwd(cwd, sizeof(cwd));
    int i=0;


    if((dir  = opendir(cwd)) == NULL) {
        perror("\nUnable to open directory.");
        exit(0);
    }
    while ((d_file=readdir(dir)) != NULL) {
        // printf("%s\n", d_file->d_name);
        char *temp=malloc(256);
        char *temp1;
        if( strncmp("test.txt",d_file->d_name,8/*strlen(filename)*/)==0){

            strcpy(temp,d_file->d_name);
            names[i]=(char *) malloc(64);
            strcpy(names[i],d_file->d_name);
            temp1=strrchr(temp,'.');
                i++;
            //  printf("TEMP %s\n",temp1);
            temp1=++temp1;

            printf("files %s\n",temp);
            printf("Names %s\n",names[i]);
            printf("TEMP %s\n",temp1);
            int n=atoi(temp1);
            printf("N %d \n",n);



        }





        free(temp);
        // free(temp1);
    }

  sort(names,broj_thread);
    for(int y=0 ;y<broj_thread;y++){
        printf("Names %s\n",names[y]);

    }
    printf("I %d\n",i);
    closedir(dir);

    // Open two files to be merged
    FILE *fp1 = fopen("test.txt.0-223", "rb");
    FILE *fp2 = fopen("test.txt.223-446", "rb");
    char buffer[BUFFER_SIZE];
    // Open file to store the result
    FILE *fp3 = fopen("full.txt", "wb");
    char c;

    if (fp1 == NULL || fp2 == NULL || fp3 == NULL)
    {
        puts("Could not open files");
        exit(0);
    }

    //  fseek(fp3,0,SEEK_END);
    // Copy contents of first file to file3.txt


    while(!feof(fp1)){
        size_t nread =fread(buffer,1,BUFFER_SIZE,fp1);
        fwrite(buffer,1,nread,fp3);
    }
    while(!feof(fp2)){
        size_t nread =fread(buffer,1,BUFFER_SIZE,fp2);
        fwrite(buffer,1,nread,fp3);
    }

    /* while ((c = fgetc(fp1)) != EOF){

         fputc(c, fp3);
     }*/


    // Copy contents of second file to file3.txt
    /*   while ((c = fgetc(fp2)) != EOF)
           fputc(c, fp3);*/

    // printf("Merged file1.txt and file2.txt into file3.txt");

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    for(int j=0;j<broj_thread;j++){
        free(names[j]);
    }

}
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
void *create_file(void * socket_tmp){

    FILE *fp;
    char buffer[BUFFER_SIZE];
    char buffer_2[BUFFER_SIZE];
    char buffer_3[BUFFER_SIZE];
    ssize_t ret=0;
    int socket=0;
    int file_size=0;
    char *filename_b=(char *)malloc(64);
    char *filename_e=(char *)malloc(64);
   static int koliko_bytes=0;
    char *filename=(char *)malloc(64);
    char *filename_f=(char *)malloc(64);
    socket = *(int *) socket_tmp;
    ret = recv(socket,buffer,BUFFER_SIZE, 0);
  //  koliko_bytes+=ret;
    printf("Return value : [%d]\n",(int)ret);
    if(ret<0){

        printf("error receving data\n %d",(int)ret);
        exit(1);
    }
    if(ret<BUFFER_SIZE){
        size_t velicina=BUFFER_SIZE;
        while(velicina>0){
            printf("[%s]\n",buffer);
            velicina-=ret;
            ret=recv(socket,buffer,velicina, 0);
            koliko_bytes+=ret;
            if(ret<0){

                printf("error receving data\n %d",(int)ret);
                exit(1);
            }
            if(ret==0){

                printf("error socket closed\n %d",(int)ret);
                exit(1);
            }
            printf("Return value %d\n",(int)ret);
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

                printf("error sending data\n %d", (int) ret);
                exit(1);
            }
            if(ret==0){

                printf("error socket closed\n %d",(int)ret);
                exit(1);
            }
        }
    }



    sscanf(buffer,"%s %d %s %s %s",filename,&file_size,filename_b,filename_e,filename_f);
    printf("File size %d\n",file_size);
    //ret = recv(socket,buffer,BUFFER_SIZE, 0);
   // printf("Return value : [%d]",(int)ret);
    if(ret<0){

        printf("error receing data\n %d",(int)ret);
        exit(1);
    }
    if(ret<BUFFER_SIZE){
        size_t velicina=BUFFER_SIZE;
        while(velicina>0){
            printf("[%s]\n",buffer);
            velicina-=ret;
            ret=recv(socket,buffer,velicina, 0);
            koliko_bytes+=ret;
            if(ret<0){

                printf("error receving data\n %d",(int)ret);
                exit(1);
            }
            if(ret==0){

                printf("error socket closed\n %d",(int)ret);
                exit(1);
            }
            printf("Return value %d\n",(int)ret);
        }
    }

    if(file_size>0){
        memset(buffer_3,0,BUFFER_SIZE);
        strcat(buffer_3,filename);
        strcat(buffer_3,".");
        strcat(buffer_3,filename_f);
       // strcat(buffer_3,filename_b);
     //   strcat(buffer_3,"-");
      //  strcat(buffer_3,filename_e);
        fp =fopen(buffer_3,"w+");
        if (fp == NULL) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        while(1){
            memset(buffer,0,BUFFER_SIZE);
            
            ret = recv(socket,buffer,BUFFER_SIZE, 0);
            printf("Return value %d\n",(int)ret);
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

                        printf("error receving data\n %d",(int)ret);
                        exit(1);
                    }
                    if(ret==0){

                        printf("socket closed\n");
                        break;
                    }
                    printf("Return value %d\n",(int)ret);
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

                        printf("error receving data\n %d", (int) ret);
                        exit(1);
                    }
                    if(ret==0){

                        printf("error socket closed\n %d",(int)ret);
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
            printf("string len of buffer %d  Buffer[%s]\n",(int) j,buffer);
            if(j==0){
                exit(1);
            }
            fwrite(buffer,1,strlen(buffer),fp);
           // fwrite(buffer,1,BUFFER_SIZE,fp);
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
   // close(socket);
    free(filename);
    free(filename_e);
    free(filename_b);
    free(filename_f);

    return 0;
};