#ifndef PROGRAM_ARGS_H
#define PROGRAM_ARGS_H

#include <pthread.h>

typedef struct
{
    char udp_ip[16];
    int udp_port;
    char tcp_ip[16];
    int tcp_port;
    char log_file[256];
    char prefix[5];
    int tcp_sockfd;
    int tcp_connected;
    pthread_mutex_t lock;
} program_args;

#endif