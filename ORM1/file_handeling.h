//
// Created by dushn on 14.12.17..
//

#ifndef ORM1_FILE_HANDELING_H
#define ORM1_FILE_HANDELING_H
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
void* file_handeling(void * socket_tmp);
int splitFile(char *fileIn, size_t maxSize);
void reassemble(int broj_sfiles,char *filename);
void *recv_files(void * socket);
void *send_filename(void *socket);
void merge();
void *new_file(void *socket_info);
void test();
void * new_connection(void * data_temp);
void *get_in_addr(struct sockaddr *sa);



struct name_s{

    int socket;
    __uint16_t thread_num;
    char filename[64];
};
struct data_s{

    int socket;
    int pack_number;
    int file_position_b;
    int file_position_e;
    int file_size;
    int numb_packets;
    char filename[64];



}data_socket;
#endif //ORM1_FILE_HANDELING_H
