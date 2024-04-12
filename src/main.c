#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "udp_handler.h"
#include "tcp_handler.h"
#include "program_args.h"

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <udp_ip:port> <tcp_ip:port> <log_file> <prefix>\n", argv[0]);
        exit(1);
    }

    program_args args;
    sscanf(argv[1], "%[^:]:%d", args.udp_ip, &args.udp_port);
    sscanf(argv[2], "%[^:]:%d", args.tcp_ip, &args.tcp_port);
    strncpy(args.log_file, argv[3], sizeof(args.log_file));
    strncpy(args.prefix, argv[4], sizeof(args.prefix));

    pthread_t udp_thread, tcp_thread;
    pthread_create(&udp_thread, NULL, udp_handler, (void*)&args);
    pthread_create(&tcp_thread, NULL, tcp_handler, (void*)&args);

    pthread_join(udp_thread, NULL);
    pthread_join(tcp_thread, NULL);

    return 0;
}