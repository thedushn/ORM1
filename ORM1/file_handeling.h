//
// Created by dushn on 14.12.17..
//

#ifndef ORM1_FILE_HANDELING_H
#define ORM1_FILE_HANDELING_H
void* file_handeling(void * socket_tmp);
int splitFile(char *fileIn, size_t maxSize);
void reassemble(int broj_sfiles,char *filename);
void *recv_files(void * socket);
void *send_files(void *socket);
void merge();

struct data_s{

    int socket;
    int pack_number;
    int file_position;
    int file_size;
    int numb_packets;
    char filename[64];



}data_socket;
#endif //ORM1_FILE_HANDELING_H
