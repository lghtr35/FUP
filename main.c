#include "include/fup.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    FUP_Client *client;
    *client = FUP_Client_Init("v1", "/FUP/Test/");

    char *file_path = FUP_Client_Download(client);

    printf("%s", file_path);

    return 0;
}
