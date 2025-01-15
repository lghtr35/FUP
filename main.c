#include "include/fup.hpp"
#include <stdio.h>

int main(int argc, char **argv)
{
    Client *client;
    *client = Init("v1", "/FUP/Test/");

    char *file_path = Download(client);

    printf("%s", file_path);

    return 0;
}
