//
// Created by dushn on 14.12.17..
//


#include <errno.h>
#include "file_handeling.h"
#define BUFFER_SIZE 1400


void * new_file(void *data_temp){

        char buffer[BUFFER_SIZE];
        char buffer2[BUFFER_SIZE];
        FILE *fp;
        ssize_t ret_1;
        char *filename=malloc(64);
        struct data_s data_s1;
        data_s1=  *((struct data_s *)data_temp);
        int socket=data_s1.socket;
        static int koliko_bytes;
    int koliko_treba;
        strcpy(filename,data_s1.filename);



        fp=fopen(data_s1.filename,"r");
        if (fp == NULL) {
            printf("open failed, errno = %d\n", errno);
            exit(1);
        }
        fseek(fp, data_s1.file_position_b,SEEK_CUR );

    if(data_s1.file_position_b!=0){
       koliko_treba=data_s1.file_position_e-data_s1.file_position_b+1;
    }
    else{

        koliko_treba=data_s1.file_position_e;
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
            memset(buffer,0,BUFFER_SIZE);
            strcpy(buffer,"end of file");
            ret_1=send(socket,buffer,BUFFER_SIZE,0);
            koliko_bytes+=ret_1;
            if(ret_1<0){
                printf("Error sending num_packets!\n\t");
                exit(1);
            }

            if(ret_1<BUFFER_SIZE) {

                size_t velicina = BUFFER_SIZE;
                while (velicina > 0) {
                    velicina -= ret_1;
                    ret_1 = send(socket, buffer, velicina, 0);
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


        size_t nread =fread(buffer,1,BUFFER_SIZE,fp);
        if(nread>koliko_treba || nread==koliko_treba){


            strncpy(buffer2,buffer,(size_t)koliko_treba);
            koliko_treba-=koliko_treba;

        }
        else{


            koliko_treba-=nread;
            strcpy(buffer2,buffer);
        }

       // (size_t)(data_s1.file_position_e-data_s1.file_position_b)
         printf("Strlen Buf %d strlen buff2 %d\n",(int)strlen(buffer),(int)strlen(buffer2));
        printf("Buffer2 [%s]\n",buffer2);




            ret_1=send(socket,buffer2,BUFFER_SIZE,0);
            koliko_bytes+=ret_1;
            printf("Return value %d\n",(int)ret_1);
            if(ret_1<0){
                printf("Error sending num_packets!\n\t");
                exit(1);
            }

            if(ret_1<strlen(buffer2)){

                size_t velicina=nread;
                while(velicina>0){
                    velicina-=ret_1;
                    ret_1=send(socket,buffer,velicina,0);
                    koliko_bytes+=ret_1;

                }
            }




        ///primamo poruku da je klient primio poruku
        memset(buffer2,0,BUFFER_SIZE);
        ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
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
                koliko_bytes += ret_1;
                if (ret_1 < 0) {

                    printf("error receing data\n %d", (int) ret_1);
                    exit(1);
                }
            }

        }
        /// ako nije dobro primljeno
        if (strcmp(buffer2, "stiglo sve") != 0) {

            printf("NOPE  \n");
            fclose(fp);
            exit(1);
        }



    }



    printf("izasli smo iz thread\n");
        return 0;
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
  //  close(socket);
    // printf("closed socket\n");
    return 0;


}
void * send_files(void *socket_tmp){

    printf("usli smo u send_files\n");
    int socket= *(int *) socket_tmp;
    char buffer[BUFFER_SIZE];

    strcpy(buffer,"hello client");
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

        printf("error receving data\n %d", (int) ret_1);
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

                printf("error receving data\n %d", (int) ret_1);
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
   // close(socket);
    // sleep(2);


    return 0;
};
void* file_handeling(void * socket_tmp){

    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    FILE *fp;
    char *filename=malloc(64);
    struct data_s data_s1;
    data_s1=  *((struct data_s *)socket_tmp);
    int socket=data_s1.socket;
    strcpy(filename,data_s1.filename);
    long	file_size;


    fp=fopen(filename,"rb");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, 0);
    memset(buffer,0,BUFFER_SIZE);
    sprintf(buffer,"%li %s " ,file_size,filename);
    printf("Buffer %s\n",buffer);
    static int koliko_bytes;
    ssize_t ret_1=send(socket,buffer,BUFFER_SIZE,0);
    if(ret_1==0){

        printf("socket closed\n");
        exit(1);
    }
    if(ret_1<0){

        printf("Error sending num_packets!\n\t");

        exit(1);
    }
    koliko_bytes+=ret_1;
    printf("file_size %li",file_size);
    memset(buffer2,0,BUFFER_SIZE);
    ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
    if(ret_1==0){

        printf("socket closed\n");
        exit(1);
    }
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
            koliko_bytes += ret_1;
            if(ret_1==0){

                printf("socket closed\n");
                exit(1);
            }
            if (ret_1 < 0) {

                printf("error receing data\n %d", (int) ret_1);
                exit(1);
            }
        }
    }

    printf("Buffer %s\n",buffer);
 /*   for(int i=0;i<(file_size/BUFFER_SIZE)+1;i++){

        fread(buffer,1,BUFFER_SIZE,fp );
        ret_1=send(socket,buffer,BUFFER_SIZE,0);

        if(ret_1<0){
            printf("Error sending num_packets!\n\t");
            exit(1);
        }

        if(ret_1<BUFFER_SIZE){

            size_t velicina=BUFFER_SIZE;
            while(velicina>0){
                velicina-=ret_1;
                ret_1=send(socket,buffer,velicina,0);

            }
        }

    }*/

    while(1){

        //printf("usli smo u nread while petlju\n");
        memset(buffer,0,BUFFER_SIZE);
        memset(buffer2,0,BUFFER_SIZE);
        size_t nread =fread(buffer,1,BUFFER_SIZE,fp);

        printf("Buffer [%s]\n",buffer);
        if(nread >0){


            ret_1=send(socket,buffer,BUFFER_SIZE,0);
            koliko_bytes+=ret_1;
            printf("Return value %d\n",(int)ret_1);
            if(ret_1==0){

                printf("socket closed\n");
                exit(1);
            }
            if(ret_1<0){
                printf("Error sending num_packets!\n\t");
                exit(1);
            }

            if(ret_1<nread){

                size_t velicina=nread;
                while(velicina>0){
                    velicina-=ret_1;
                    ret_1=send(socket,buffer,velicina,0);
                    koliko_bytes+=ret_1;

                }
            }


        }
        if(nread < BUFFER_SIZE){

            if(feof(fp)){
                printf("end of file \n");
                printf("the file was sent to %d\n",socket);



            }
            if (ferror(fp)){
                printf("Error reading\n");
            }

            //printf("%d",count);
            break;
        }

        memset(buffer2,0,BUFFER_SIZE);
        ret_1 =recv(socket,buffer2,BUFFER_SIZE,0);
        if(ret_1==0){

            printf("socket closed\n");
            exit(1);
        }
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
                koliko_bytes += ret_1;
                if(ret_1==0){

                    printf("socket closed\n");
                    exit(1);
                }
                if (ret_1 < 0) {

                    printf("error receing data\n %d", (int) ret_1);
                    exit(1);
                }
            }

        }
        if (strcmp(buffer2, "stiglo sve") != 0) {

            printf("NOPE  \n");
            fclose(fp);
            exit(1);
        }


    }

    printf("poslali file\n");
    fclose(fp);
    memset(buffer,0,BUFFER_SIZE);
    strcpy(buffer,"end of file");
    ret_1=send(socket,buffer,BUFFER_SIZE,0);
    koliko_bytes+=ret_1;
    if(ret_1<0){
        printf("Error sending num_packets!\n\t");
        exit(1);
    }

    if(ret_1<BUFFER_SIZE) {

        size_t velicina = BUFFER_SIZE;
        while (velicina > 0) {
            velicina -= ret_1;
            ret_1 = send(socket, buffer, velicina, 0);
            koliko_bytes+=ret_1;
            if (ret_1 < 0) {

                printf("error receing data\n %d", (int) ret_1);
                exit(1);
            }
        }

    }
    printf("number of bytes sent %d",koliko_bytes);
    printf("end of file  msg\n");
    ret_1 =recv(socket,buffer,BUFFER_SIZE,0);
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
            koliko_bytes += ret_1;
            if (ret_1 < 0) {

                printf("error receing data\n %d", (int) ret_1);
                exit(1);
            }
        }

    }
    if (strcmp(buffer, "stiglo sve") == 0) {

        printf("proslo sve kako treba \n");
    }

return 0;
};
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

void reassemble(int broj_sfiles,char *filename ){

    DIR *dir;
    struct dirent *d_file;
    char cwd[1024];
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
       if( strncmp(filename,d_file->d_name,strlen(filename))==0){
            i++;
           strcpy(temp,d_file->d_name);
           temp1=strrchr(temp,'.');
         //  printf("TEMP %s\n",temp1);
           temp1=++temp1;
           printf("TEMP %s\n",temp1);
           int n=atoi(temp1);
           printf("N %d \n",n);


       }





        free(temp);
       // free(temp1);
    }
    printf("I %d\n",i);
    closedir(dir);

}
void merge (){



    // Open two files to be merged
    FILE *fp1 = fopen("rc.jpg.temp2", "rb");
    FILE *fp2 = fopen("rc.jpg.5", "rb");
    char buffer[BUFFER_SIZE];
    // Open file to store the result
    FILE *fp3 = fopen("rc.jpg.temp3", "wb");
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
}




