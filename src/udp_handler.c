#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "udp_handler.h"
#include "program_args.h"

void* udp_handler(void* args)
{
    program_args* pargs = (program_args*)args;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[128];
    FILE* log_file = fopen(pargs->log_file, "a");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        fprintf(log_file, "UDP socket creation failed\n");
        fclose(log_file);
        return NULL;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(pargs->udp_port);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(log_file, "UDP socket bind failed\n");
        close(sockfd);
        fclose(log_file);
        return NULL;
    }

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&cliaddr, &len);
        if (n < 0)
        {
            fprintf(log_file, "UDP data receive failed\n");
            continue;
        }

        if (n >= 16 && n <= 128)
        {
            // Check if the TCP connection is established
            if (pargs->tcp_connected)
            {
                // Add the prefix to the received data
                char modified_data[132];
                snprintf(modified_data, sizeof(modified_data), "%s%.*s", pargs->prefix, n, buffer);

                // Send the modified data to the TCP handler
                ssize_t bytes_sent = send(pargs->tcp_sockfd, modified_data, strlen(modified_data), 0);
                if (bytes_sent != (ssize_t)strlen(modified_data))
                {
                    fprintf(log_file, "UDP data send to TCP handler failed\n");
                }
                else
                {
                    fprintf(log_file, "UDP data sent to TCP handler\n");
                }
            }
            else
            {
                fprintf(log_file, "UDP data discarded due to TCP connection not established\n");
            }
        }
        else
        {
            fprintf(log_file, "Received UDP data block of invalid size: %d bytes\n", n);
            fflush(log_file);
            printf("Received UDP data block of invalid size: %d bytes\n", n);
        }
    }

    close(sockfd);
    fclose(log_file);
    return NULL;
}