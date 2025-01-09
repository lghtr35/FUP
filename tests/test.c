#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    struct addrinfo hints;        // Hints or "filters" for getaddrinfo()
    struct addrinfo *res;         // Result of getaddrinfo()
    struct addrinfo *r;           // Pointer to iterate on results
    int status;                   // Return value of getaddrinfo()
    char buffer[INET_ADDRSTRLEN]; // Buffer to convert IP address

    memset(&hints, 0, sizeof hints); // Initialize the structure
    hints.ai_family = AF_UNSPEC;     // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP

    if (argc < 3)
    {
        return (1);
    }

    // Get the associated IP address(es)
    status = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (status != 0)
    { // error !
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return (2);
    }

    printf("IP adresses for %s:\n", argv[1]);

    r = res;
    while (r != NULL)
    {
        void *addr; // Pointer to IP address
        if (r->ai_family == AF_INET)
        { // IPv4
            // we need to cast the address as a sockaddr_in structure to
            // get the IP address, since ai_addr might be either
            // sockaddr_in (IPv4) or sockaddr_in6 (IPv6)
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)r->ai_addr;
            // Convert the integer into a legible IP address string
            inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer);
            printf("IPv4: %s\n", buffer);
        }
        else
        { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)r->ai_addr;
            inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof buffer);
            printf("IPv6: %s\n", buffer);
        }
        r = r->ai_next; // Next address in getaddrinfo()'s results
    }
    freeaddrinfo(res); // Free memory
    return (0);
}