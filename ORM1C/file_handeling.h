//
// Created by dushn on 14.12.17..
//

#ifndef ORM1C_FILE_HANDELING_H
#define ORM1C_FILE_HANDELING_H
void *create_file(void *socket_tmp);
void * send_files(void *socket_tmp);
void *recv_files(void *socket_tmp);
void merge (const int broj_thread ,char * filename);
const int get_filename(void * socket_tmp);
char name[256];

#endif //ORM1C_FILE_HANDELING_H
