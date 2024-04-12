#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp_handler.h"
#include "program_args.h"

void* tcp_handler(void* args)
{
    program_args* pargs = (program_args*)args;
    int sockfd;
    struct sockaddr_in servaddr;
    FILE* log_file = fopen(pargs->log_file, "a");

    while (1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            fprintf(log_file, "TCP socket creation failed\n");
            break;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(pargs->tcp_port);
        if (inet_pton(AF_INET, pargs->tcp_ip, &servaddr.sin_addr) <= 0)
        {
            fprintf(log_file, "Invalid TCP server address\n");
            close(sockfd);
            break;
        }

        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        {
            fprintf(log_file, "TCP connection failed\n");
            close(sockfd);
            pargs->tcp_connected = 0;
            sleep(1);
            continue;
        }

        fprintf(log_file, "TCP connection established\n");
        pargs->tcp_sockfd = sockfd;
        pargs->tcp_connected = 1;

        while (1)
        {
            char data[132];
            ssize_t bytes_received = recv(sockfd, data, sizeof(data), 0);
            if (bytes_received <= 0)
            {
                fprintf(log_file, "TCP connection closed by the server\n");
                break;
            }

            // Ignore any data received from the TCP server
        }

        close(sockfd);
        pargs->tcp_connected = 0;
    }

    fclose(log_file);
    return NULL;
}