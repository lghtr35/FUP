#include "facade/server.hpp"
#include "facade/client.hpp"

int main(int argc, char *argv[])
{
    fup::facade::server server(4000);
    return 0;
}