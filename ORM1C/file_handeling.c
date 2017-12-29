//
// Created by dushn on 14.12.17..
//

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "file_handeling.h"

#define BUFFER_SIZE 1400
#define BUFFER_SIZE2 1404
static int myCompare (const void * a, const void * b)
{
    return strcmp (*(const char **) a, *(const char **) b);
}

void sort( char *arr[], int n)
{
    qsort (arr, n, sizeof (const char *), myCompare);
}
void merge (const int broj_thread,char *filename){



    DIR *dir;
    struct dirent *d_file;
    char cwd[1024];
    char *names[broj_thread+1];
    names[broj_thread]=(char *) malloc(64);

    //char (*file_name_array)[64];
    getcwd(cwd, sizeof(cwd));
    int t=0;


    if((dir  = opendir(cwd)) == NULL) {
        perror("\nUnable to open directory.");
        exit(0);
    }
    while ((d_file=readdir(dir)) != NULL) {
        // printf("%s\n", d_file->d_name);
        char *temp=malloc(256);
        char *temp1;
        if( strncmp(filename,d_file->d_name,strlen(filename))==0){

            strcpy(temp,d_file->d_name);
            names[t]=(char *) malloc(64);
            strcpy(names[t],d_file->d_name);
            temp1=strrchr(temp,'.');
            printf("Names %s\n",names[t]);
                t++;
            //  printf("TEMP %s\n",temp1);
            temp1=++temp1;

            printf("files %s\n",temp);

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
    printf("I %d\n",t);
    closedir(dir);

    // Open two files to be merged
    FILE *fp_temp1;
 while((fp_temp1= fopen(names[1], "rb"))!=NULL){
        FILE *fp_temp;

        FILE *fp_final;
        int errnum;
        char buffer[BUFFER_SIZE];
     static int brojac =2;
     fp_temp= fopen(names[0], "rb");
     printf("Trying to open %s\n",names[0]);
     char *temp=malloc(256);
     char *temp1;
        if (fp_temp == NULL) {

            free(temp);
            errnum = errno;

            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error printed by perror");
            fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
        }
       /* fp_temp1= fopen(names[1], "rb");
        printf("Trying to open %s\n",names[1]);
        if (fp_temp1 == NULL) {

            errnum = errno;
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error printed by perror");
            fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
        }*/

     char name_buffer[64];
     strcpy(temp,names[1]);
     temp1=strrchr(temp,'.');
     temp1=++temp1;
     memset(name_buffer,0,64);
     strcpy(name_buffer,names[0]);
     strcat(name_buffer,temp1);
    fp_final= fopen(name_buffer, "wb");
     if (fp_final == NULL) {


         errnum = errno;
         printf("Trying to open %s\n",name_buffer);
         fprintf(stderr, "Value of errno: %d\n", errno);
         perror("Error printed by perror");
         fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
     }

     while(!feof(fp_temp)){
         size_t nread =fread(buffer,1,BUFFER_SIZE,fp_temp);
         fwrite(buffer,1,nread,fp_final);
     }
     while(!feof(fp_temp1)){
         size_t nread =fread(buffer,1,BUFFER_SIZE,fp_temp1);
         fwrite(buffer,1,nread,fp_final);
     }

     fclose(fp_temp);
     fclose(fp_temp1);
     fclose(fp_final);

    int ret = remove(names[1]);

     if(ret == 0) {
         printf("File deleted successfully\n");
     } else {
         printf("Error: unable to delete the file\n");
     }
      ret = remove(names[0]);

     if(ret == 0) {
         printf("File deleted successfully\n");
     } else {
         printf("Error: unable to delete the file\n");
     }
     free(temp);

     memset(names[0],0,64);
    // names[0]="final";
     strcpy(names[0],name_buffer);
   /*  if(broj_thread==brojac){

         names[1]=={""};
     }*/
         names[1]=names[brojac];
         brojac++;





    }
/*    char *final;
    char *temp3;
    strcpy(temp3,names[0]);*/

    rename(names[0],filename);
    for(int i=0;i<=broj_thread;i++){

        free(names[i]);

    }
    printf("spojili smo \n");



}

void *create_file(void * socket_tmp){

    FILE *fp;
    char buffer[BUFFER_SIZE];
    char buffer_2[BUFFER_SIZE];
    char buffer_3[BUFFER_SIZE];
    char buffer_4[BUFFER_SIZE2];
    ssize_t ret=0;
    int socket=0;
    int file_size=0;
    int fp1;
    int upisano_temp;
    ssize_t  koliko_treba_upisati;
   /* char *filename_b=(char *)malloc(64);
    char *filename_e=(char *)malloc(64);*/
    int filename_b=0;
    int filename_e=0;
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



    sscanf(buffer,"%s %d %d %d %s",filename,&file_size,&filename_b,&filename_e,filename_f);
    printf("File size %d\n",file_size);
    koliko_treba_upisati=(filename_e-filename_b);
    //ret = recv(socket,buffer,BUFFER_SIZE, 0);
   // printf("Return value : [%d]",(int)ret);






    if(file_size>0){
        memset(buffer_3,0,BUFFER_SIZE);
        strcat(buffer_3,filename);
        strcat(buffer_3,".");
        strcat(buffer_3,filename_f);
       // strcat(buffer_3,filename_b);
     //   strcat(buffer_3,"-");
      //  strcat(buffer_3,filename_e);
        fp =fopen(buffer_3,"wb");
      //  fp1=open(buffer_3, O_RDWR | O_APPEND | O_CREAT | O_TRUNC,S_IWRITE | S_IREAD);

      //  if (fp1 <0) {
             if (fp == NULL) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        while(1){
            memset(buffer,0,BUFFER_SIZE);
            memset(buffer_4,0,BUFFER_SIZE2);
            ret = recv(socket,buffer_4,BUFFER_SIZE2, 0);
            printf("Return value %d\n",(int)ret);
            koliko_bytes+=ret;
            if(ret<BUFFER_SIZE2){
                size_t velicina=BUFFER_SIZE2;
                velicina-=ret;
                while(velicina>0 || velicina <0){
                    printf("Buffer [%s]\n",buffer_4);

                    ret=recv(socket,buffer_4,velicina, 0);
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
          //  printf("buffer4:  [%s]\n",buffer_4);

            int broj_bites;

            // sscanf(buffer_4,"%d ",&broj_bites);
            char broj_bites_c[4];
            memcpy(broj_bites_c+0,buffer_4,sizeof(broj_bites_c));
            sscanf(broj_bites_c,"%d",&broj_bites);
        //    memcpy(buffer+0,buffer_4+4,(size_t)broj_bites);
            memcpy(buffer+0,buffer_4+4,BUFFER_SIZE);

            memset(buffer_2,0,BUFFER_SIZE);
           // strcpy(buffer_2,buffer);
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

           // printf("Buffer %s\n",buffer);
            if( (t=strcmp(buffer,"end of file"))==0) {
                printf("dosli smo do kraja file\n");
                fclose(fp);
                //close(fp1);
                break;
            }

          /*  if(buffer[0]==0){

                printf("kraj file\n");
                fclose(fp);
               // exit(1);.
                break;
            }*/
          //  ssize_t j= strlen(buffer);
         //   printf("string len of buffer %d  Buffer[%s]\n",(int) j,buffer);



            int write1 =(int)fwrite (buffer, 1,(size_t)broj_bites,fp);
            /*= (int)write(fp1,buffer,(size_t)broj_bites);*/
            printf("write %d \n",write1);
            upisano_temp+=write1;
          /*  koliko_treba_upisati-=write1;
            printf("koliko treba %d \n",koliko_treba_upisati);
            if(koliko_treba_upisati==0){
                fclose(fp);
            }
            else{
                write1= fwrite(buffer,1,BUFFER_SIZE,fp);
            }*/

        }

    }



    //fclose(fp);
    printf("Buffer [%s]\n",buffer);
    printf("koliko bytes upisano   %d\n",(int)upisano_temp);
    printf("File size              %d\n",file_size);
    printf("koliko bytes primljeno %d\n",koliko_bytes);

    fp=fopen(buffer_3,"r");

    fseek(fp, 0, SEEK_END);
    file_size =(int) ftell(fp);
    //file_size=15555;
    fseek(fp, 0, 0);

    fclose(fp);
    /// u zavisnosti od broja konekcija delimo file na toliko delova  za sada 4
    printf("file_size %d \n",file_size);

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
   // free(filename_e);
   // free(filename_b);
    free(filename_f);

    return 0;
};