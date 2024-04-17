#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "udp_handler.h"
#include "program_args.h"

extern volatile sig_atomic_t terminate;

void* udp_handler(void* args)
{
    program_args* pargs = (program_args*)args;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[132];
    FILE* log_file = fopen(pargs->log_file, "a");
    if (log_file == NULL)
    {
        fprintf(stderr, "Failed to open log file\n");
        return NULL;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        fprintf(log_file, "UDP socket creation failed\n");
        fclose(log_file);
        return NULL;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, pargs->udp_ip, &servaddr.sin_addr) <= 0)
    {
        fprintf(log_file, "Invalid UDP server address\n");
        close(sockfd);
        fclose(log_file);
        return NULL;
    }
    servaddr.sin_port = htons(pargs->udp_port);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(log_file, "UDP socket bind failed\n");
        close(sockfd);
        fclose(log_file);
        return NULL;
    }

    while (!terminate)
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
            printf("[udp_handler.c] Received UDP data: ");
            for (int i = 0; i < n; i++)
            {
                printf("%02X ", (unsigned char)buffer[i]);
            }
            printf("\n");

            pthread_mutex_lock(&pargs->lock);
            int tcp_sockfd = pargs->tcp_sockfd;
            int tcp_connected = pargs->tcp_connected;
            pthread_mutex_unlock(&pargs->lock);

            if (tcp_connected)
            {
                size_t prefix_len = strlen(pargs->prefix);
                size_t modified_data_size = prefix_len + n;
                char* modified_data = malloc(modified_data_size);
                if (modified_data == NULL)
                {
                    fprintf(log_file, "Memory allocation failed\n");
                    continue;
                }
                memcpy(modified_data, pargs->prefix, prefix_len);
                memcpy(modified_data + prefix_len, buffer, n);

                const char* data_to_send = modified_data;
                size_t total_bytes_to_send = modified_data_size;
                size_t total_bytes_sent = 0;

                printf("[udp_handler.c] Sending TCP data: ");
                for (size_t i = 0; i < total_bytes_to_send; i++)
                {
                    printf("%02X ", (unsigned char)data_to_send[i]);
                }
                printf("\n");

                pthread_mutex_lock(&pargs->lock);
                while (total_bytes_sent < total_bytes_to_send)
                {
                    ssize_t bytes_sent = send(tcp_sockfd, data_to_send + total_bytes_sent, total_bytes_to_send - total_bytes_sent, 0);
                    if (bytes_sent <= 0)
                    {
                        fprintf(log_file, "UDP data send to TCP handler failed\n");
                        break;
                    }
                    total_bytes_sent += bytes_sent;
                }
                pthread_mutex_unlock(&pargs->lock);

                if (total_bytes_sent == total_bytes_to_send)
                {
                    fprintf(log_file, "UDP data sent to TCP handler\n");
                }
                else
                {
                    fprintf(log_file, "UDP data send to TCP handler failed\n");
                }

                free(modified_data);
            }
            else
            {
                fprintf(log_file, "UDP data discarded due to TCP connection not established\n");
            }
        }
        else
        {
            fprintf(log_file, "Received UDP data block of invalid size: %d bytes\n", n);
            fprintf(log_file, "UDP data discarded\n");
        }

        pthread_mutex_lock(&pargs->lock);
        int should_terminate = terminate;
        pthread_mutex_unlock(&pargs->lock);
        
        if (should_terminate)
            break;
    }

    close(sockfd);
    fclose(log_file);
    return NULL;
}