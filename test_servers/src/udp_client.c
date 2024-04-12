#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 128

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1)
    {
        printf("Enter data to send (16 to 128 bytes) or 'q' to exit: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        size_t data_size = strlen(buffer);

        // Remove the trailing newline character
        if (data_size > 0 && buffer[data_size - 1] == '\n')
        {
            buffer[data_size - 1] = '\0';
            data_size--;
        }

        if (strcmp(buffer, "q") == 0)
        {
            printf("Exiting UDP client.\n");
            break;
        }

        // if (data_size < 16 || data_size > 128)
        // {
        //     printf("Invalid data size. Please enter data between 16 and 128 bytes.\n");
        //     continue;
        // }

        if (sendto(sockfd, buffer, data_size, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto failed");
            break;
        }

        printf("Data sent: %s\n", buffer);

        // Sleep for a short interval before sending the next data block
        sleep(1);
    }

    close(sockfd);
    return 0;
}