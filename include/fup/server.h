#ifndef FUP_SERVER_H
#define FUP_SERVER_H

#include "common.h"
#include "message.h"
#define DEFAULT_TCP_PORT 12420
#define DEFAULT_UDP_PORT 12421

typedef struct FUP_Server
{
    FUP_Version version;
    int max_connection_count;
    int connection_count;
    FUP_Connection_Id *connections;
    int tcp_socket; // id for socket acquired from socket()
    int file_count;
    char *save_location;
    char **filenames;
} FUP_Server;

FUP_Server FUP_Server_Init(int version, int max_conn_count, char *save_location);
void FUP_Server_Delete(FUP_Server *self);

void FUP_Server_Listen();

#endif