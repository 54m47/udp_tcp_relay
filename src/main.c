#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "udp_handler.h"
#include "tcp_handler.h"
#include "program_args.h"

volatile sig_atomic_t terminate = 0;

void signal_handler(int signum)
{
    __atomic_store_n(&terminate, 1, __ATOMIC_SEQ_CST);
}

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <udp_ip:port> <tcp_ip:port> <log_file> <prefix>\n", argv[0]);
        exit(1);
    }

    program_args args;
    if (sscanf(argv[1], "%15[^:]:%d", args.udp_ip, &args.udp_port) != 2 ||
        sscanf(argv[2], "%15[^:]:%d", args.tcp_ip, &args.tcp_port) != 2)
    {
        fprintf(stderr, "Invalid UDP or TCP address format\n");
        exit(1);
    }
    snprintf(args.log_file, sizeof(args.log_file), "%s", argv[3]);
    snprintf(args.prefix, sizeof(args.prefix), "%s", argv[4]);

    pthread_mutex_init(&args.lock, NULL);

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    pthread_t udp_thread, tcp_thread;
    if (pthread_create(&udp_thread, NULL, udp_handler, (void*)&args) != 0)
    {
        fprintf(stderr, "Failed to create UDP thread\n");
        pthread_mutex_destroy(&args.lock);
        exit(1);
    }
    if (pthread_create(&tcp_thread, NULL, tcp_handler, (void*)&args) != 0)
    {
        fprintf(stderr, "Failed to create TCP thread\n");
        pthread_cancel(udp_thread);
        pthread_join(udp_thread, NULL);
        pthread_mutex_destroy(&args.lock);
        exit(1);
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    int sig;
    sigwait(&mask, &sig);

    pthread_mutex_lock(&args.lock);
    terminate = 1;
    pthread_mutex_unlock(&args.lock);

    pthread_cancel(udp_thread);
    pthread_cancel(tcp_thread);

    if (pthread_join(udp_thread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join UDP thread\n");
    }
    if (pthread_join(tcp_thread, NULL) != 0)
    {
        fprintf(stderr, "Failed to join TCP thread\n");
    }

    pthread_mutex_destroy(&args.lock);

    return 0;
}