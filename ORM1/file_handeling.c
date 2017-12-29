//
// Created by dushn on 14.12.17..
//


#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <netinet/in.h>
#include "file_handeling.h"
#define BUFFER_SIZE 1400
#define BUFFER_SIZE2 1404


void * new_file(void *data_temp){

        char buffer[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        char buffer3[BUFFER_SIZE2];
        char write_b[4];
        FILE *fp;
    FILE * fp_temp;
        int fd;
        int fp2;
        ssize_t ret_1;
        char *filename=malloc(64);
        struct data_s data_s1;
        data_s1=  *((struct data_s *)data_temp);
        int socket=data_s1.socket;
        static int koliko_bytes;
    int koliko_treba;
    int koliko_treba_1;
    size_t read_temp;
        strcpy(filename,data_s1.filename);



        fp=fopen(data_s1.filename,"rb");
        fp_temp=fopen("rc1.jpg","wb");

      //  fp=fopen(data_s1.filename,"rb");
    //fd= open(data_s1.filename,O_RDONLY,S_IREAD);
   // fp2=creat("test.jpg",);
   // fp2=open("test1.txt",O_WRONLY| O_CREAT,S_IWRITE);
   /* if(fd<=0){
        printf("open failed, errno = %d\n", errno);
        exit(1);

    }*/
        if (fp == NULL || fp_temp==NULL) {
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






    sprintf(buffer,"%s %d  %d %d %d" ,filename,koliko_treba,data_s1.file_position_b,data_s1.file_position_e,data_s1.pack_number);
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
            if (ret < 0) {

                printf("error sending data\n %d", (int) ret);
                exit(1);
            }
        }
    }
    printf("Return value %d\n",(int)ret);

    memset(buffer2,0,BUFFER_SIZE);
    ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
    if (ret_1 < 0) {

        printf("error receving data\n %d", (int) ret_1);
        exit(1);
    }
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
                exit(1);
            }
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);
                exit(1);
            }
        }
        printf("Return value %d\n",(int)ret_1);

    }

    if (strcmp(buffer2, "stiglo sve") != 0) {

        printf("NOPE  \n");
        // fclose(fp);
        exit(1);
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
                exit(1);
            }

            if(ret_1<BUFFER_SIZE2) {

                size_t velicina = BUFFER_SIZE2;
                while (velicina > 0) {
                    velicina -= ret_1;
                    ret_1 = send(socket, buffer3, velicina, 0);
                    koliko_bytes+=ret_1;
                    if (ret_1 < 0) {

                        printf("error receving data\n %d", (int) ret_1);
                        fclose(fp);
                        exit(1);
                    }
                }

            }

            printf("end of file  msg\n");
            ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
            if (ret_1 < 0) {

                printf("error receving data\n %d", (int) ret_1);
                fclose(fp);
                exit(1);
            }
            if(ret_1<BUFFER_SIZE) {
                size_t velicina = BUFFER_SIZE;
                velicina -= ret_1;
                while (velicina > 0 || velicina < 0) {
                    printf("Buffer [%s]\n", buffer);

                    ret_1 = recv(socket, buffer, velicina, 0);
                    velicina -= ret_1;
                    koliko_bytes += ret_1;
                    if (ret_1 < 0) {

                        printf("error receing data\n %d", (int) ret_1);
                        fclose(fp);
                        exit(1);
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
        /*     size_t nread =fread(buffer,1,BUFFER_SIZE,fp);
             printf("Buffer %s \n",buffer);
              ssize_t read1=  read(fd,buffer4,BUFFER_SIZE);
             printf("Buffer4 %s \n",buffer4);
              ssize_t  write1=  write(fp2,buffer4,(size_t)read1);*/


       // nread=(size_t)read(fd,buffer,BUFFER_SIZE);
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
         printf("Strlen Buf %d strlen buff2 %d\n",(int)strlen(buffer),(int)strlen(buffer2));
        printf("Buffer2 [%s]\n",buffer2);


        sprintf(write_b,"%" SCNu16 "",(__uint16_t)nread);
        memset(buffer3,0,BUFFER_SIZE2);
        memcpy(buffer3+0, write_b,sizeof(write_b));
        memcpy(buffer3+4,buffer2,BUFFER_SIZE);
                //sprintf(buffer3,"%" SCNu16 "",packet_size);
        printf("Strlen Buf3 %d \n",(int)strlen(buffer3));


        ret_1=send(socket,buffer3,BUFFER_SIZE2,0);

        printf("Return value %d\n",(int)ret_1);
        if(ret_1<0){
            printf("Error sending num_packets!\n\t");
            exit(1);
        }
        if(ret_1==0){
            printf("Error sending num_packets!\n\t");
            printf("socket closed\n");
            exit(1);
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

                    printf("error receing data\n %d", (int) ret_1);
                    exit(1);
                }
            }
        }





        ///primamo poruku da je klient primio poruku
     //   memset(buffer2,0,BUFFER_SIZE);
        memset(buffer,0,BUFFER_SIZE);
        ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
        if (ret_1 < 0) {

            printf("error receving data\n %d", (int) ret_1);
            exit(1);
        }
        if(ret_1<BUFFER_SIZE) {
            size_t velicina = BUFFER_SIZE;
            velicina -= ret_1;
            while (velicina > 0 || velicina < 0) {
                printf("Buffer2 [%s]\n", buffer);

                ret_1 = recv(socket, buffer, velicina, 0);
                velicina -= ret_1;
                koliko_bytes += ret_1;
                if (ret_1 < 0) {

                    printf("error receving data\n %d", (int) ret_1);
                    exit(1);
                }
            }

        }
        /// ako nije dobro primljeno
        if(strcmp(buffer, "stiglo sve") == 0){

            char buffer_temp [BUFFER_SIZE];
            memset(buffer_temp,0,BUFFER_SIZE);
            memcpy(buffer_temp,buffer3+4,BUFFER_SIZE);

          size_t write_temp= fwrite(buffer_temp,1,nread,fp_temp);

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
    close(fd);
    close(fp2);
    fclose(fp_temp);
        return 0;
}


/*
**  splitFile
**  Splits an existing input file into multiple output files with a specified
**  maximum file size.
**
**  Return Value:
**  Number of created result files, or 0 in case of bad input data or a negative
**  value in case of an error during file splitting.
*/
int splitFile(char *fileIn, size_t maxSize)
{
    int result = 0;
    FILE *fIn;
    FILE *fOut;
    char buffer[1024 * 16];
    size_t size=0;
    size_t read;
    size_t written;


    if ((fileIn != NULL) && (maxSize > 0))
    {
        fIn = fopen(fileIn, "rb");
        if (fIn != NULL)
        {
            fOut = NULL;
            result = 1;   /* we have at least one part */

            while (!feof(fIn))
            {
                /* initialize (next) output file if no output file opened */
                if (fOut == NULL)
                {

                    sprintf(buffer, "%s.%d", fileIn, result);
                    fOut = fopen(buffer, "wb");
                    if (fOut == NULL)
                    {
                        result *= -1;
                        break;
                    }

                    size = 0;
                }

                /* calculate size of data to be read from input file in order to not exceed maxSize */
                read = sizeof(buffer);
                if ((size + read) > maxSize)
                {
                    read = maxSize - size;
                }

                /* read data from input file */
                read = fread(buffer, 1, read, fIn);
                if (read == 0)
                {
                    result *= -1;
                    break;
                }

                /* write data to output file */
                written = fwrite(buffer, 1, read, fOut);
                if (written != read)
                {
                    result *= -1;
                    break;
                }

                /* update size counter of current output file */
                size += written;
                if (size >= maxSize)   /* next split? */
                {
                    fclose(fOut);
                    fOut = NULL;
                    result++;
                }
            }

            /* clean up */
            if (fOut != NULL)
            {
                fclose(fOut);
            }
            fclose(fIn);
        }
    }

    return (result);
}
void merge(){


    FILE *fp ;
    FILE * fp1;
    char buffer [BUFFER_SIZE];
    fp=fopen("rc.jpg","rb");

    if(fp==NULL){


        printf("file coundnt get opened\n");
        exit(1);

    }
    fp1=fopen("new.jpg","wb");
    if(fp1==NULL){


        printf("file coundnt get opened\n");
        exit(1);

    }




    fclose(fp);
    fclose(fp1);

}

void test(){


    int fp,fp1 ;
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE2];
    char write_b[4];
    fp= open("rc.jpg",O_RDONLY,S_IREAD);
    // fp2=creat("test.jpg",);
    fp1=open("rc1.jpg", O_RDWR | O_APPEND | O_CREAT | O_TRUNC,S_IWRITE | S_IREAD);
    if(fp1<0 || fp<0){

        printf("open failed, errno = %d\n", errno);
        printf("file opening failed fp %d fp1 %d \n",fp,fp1);
        close(fp);
        close(fp1);
        exit(1);

    }
    size_t  read1;
    while ((read1=(size_t)read(fp,buffer,BUFFER_SIZE))>0){


        uint16_t write1 = (uint16_t)write(fp1,buffer,read1);
        uint16_t write_test=0;

        sprintf(write_b,"%" SCNu16 "",write1);

        memset(buffer2,0,BUFFER_SIZE2);
        memcpy(buffer2+0, write_b, strlen(write_b));
        printf("BUffer2 %s\n",buffer2);
        memcpy(buffer2+4,buffer,BUFFER_SIZE);
        printf("BUffer2 %s\n",buffer2);
        printf("BUffer %s\n",buffer);
        char b[4];
        memcpy(b+0,buffer2+0,sizeof(b));
        sscanf(b,"%" SCNu16 "",&write_test);
        char b2[BUFFER_SIZE];
        memset(b2,0,BUFFER_SIZE);
        memcpy(b2+0,buffer2+4,BUFFER_SIZE);
        printf("B2 %s\n",b2);
        int t= strcmp(buffer,b2);

        printf("write %d \n",write1);

    }

    close(fp);
    close(fp1);

}



