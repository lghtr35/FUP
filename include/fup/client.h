#ifndef FUP_CLIENT_H
#define FUP_CLIENT_H

#include "common.h"
#include "message.h"

/*
 * Can download one file at a time
 * Might need refactoring to allow more versions of FUP to be implemented as submodules
 */

typedef struct FUP_Client
{
    FUP_Version version; // has to have version
    char *save_location;
} FUP_Client;

FUP_Client FUP_Client_Init(int version, char *save_location);
void FUP_Client_Delete(FUP_Client *self);

// Operations possible from client
int FUP_Client_Upload(FUP_Client *self, char *destination_url, int destination_port, char *file_fullpath, int *res);
int FUP_Client_Download(FUP_Client *self, char *destination_url, int destination_port, char *filename, int *res);
int FUP_Client_ListFiles(FUP_Client *self, char *destination_url, int destination_port, char **res);

// Kind of private/underlying functions
int _FUP_Client_Connect(FUP_Client *self, int ip_family, char *destination_url, unsigned short destination_port, FUP_Keyword message_type);
int _FUP_Client_Send(FUP_Client *self, unsigned int destination_ip, unsigned short destination_port, const FUP_Message request);
int _FUP_Client_Receive(FUP_Client *self, FUP_Message *response);
int _FUP_Client_Disconnect(FUP_Client *self, int socket_fd);

#endif