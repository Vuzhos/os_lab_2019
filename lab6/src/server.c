#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "multmod.h"

uint64_t Factorial(int begin, int end, int mod) {
    uint64_t ans = 1;
    for (int i = begin; i < end; ++i) {
        ans = MultModulo(ans, i % mod, mod);
    }
    return ans;
}

void* ThreadFactorial(struct FactorialArgs* args) {
    return (void *)(uint64_t *)Factorial(args->begin, args->end, args->mod);
}

int main(int argc, char **argv) {
    int tnum = -1; 
    int port = -1;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"port", required_argument, 0, 0},
            {"tnum", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1) { break; }

        switch (c) {
            case 0: {
                switch (option_index) {
                    case 0:
                        port = atoi(optarg); 
                        if (port < 0 || port > 65535) {
                            printf("Error! port must be positive and less than 65536!\n");
                            return 1;
                        } break;

                    case 1:
                        tnum = atoi(optarg);
                        if (tnum < 1) {
                            printf("Error! tnum must be positive!\n");
                            return 1;
                        } break;

                    default:
                        fprintf(stderr, "Index %d is out of options\n", option_index);
                    }
                } break;

            case '?':
                fprintf(stderr, "Unknown argument\n");
                break;

            default:
                fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (port == -1 || tnum == -1) {
        fprintf(stderr, "Using: %s --port \"num\" --tnum \"num\" \n", argv[0]);
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "Can not create server socket!");
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t)port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (err < 0) {
        fprintf(stderr, "Can not bind to socket!");
        return 1;
    }

    err = listen(server_fd, 128);
    if (err < 0) {
        fprintf(stderr, "Could not listen on socket\n");
        return 1;
    }

    printf("Server listening at %d\n", port);

    while (true) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        // Установление соединения с клиентом
        if (client_fd < 0) {
            fprintf(stderr, "Could not establish new connection\n");
            continue;
        }

        while (true) {
            unsigned int buffer_size = sizeof(uint64_t) * 3;
            char from_client[buffer_size];
            int read = recv(client_fd, from_client, buffer_size, 0);

            // Получение данных от клиента
            if (!read)
                break; 
            if (read < 0) {
                fprintf(stderr, "Client read failed\n");
                break;
            }
            if (read < buffer_size) {
                fprintf(stderr, "Client send wrong data format with size: %d, when expected: %d\n", read, buffer_size);
                break;
            }

            pthread_t threads[tnum];

            uint64_t begin = 0;
            uint64_t end = 0;
            uint64_t mod = 0;
            memcpy(&begin, from_client, sizeof(uint64_t));
            memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
            memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

            fprintf(stdout, "Receive: %lu %lu %lu\n", begin, end, mod); 
            
            struct FactorialArgs args[tnum];
            int step = (end-begin) / tnum;

            for (uint32_t i = 0; i < tnum; i++) {
                {
                    args[i].mod = mod;
                    args[i].begin = begin + i * step;
                    if (i == tnum - 1) {
                        args[i].end = end + 1;
                    } else {
                        args[i].end = args[i].begin + step;
                    }
                }
                if (pthread_create(&threads[i], NULL, (void*)ThreadFactorial, (void *)&args[i])) {
                    printf("Error: pthread_create failed!\n");
                    return 1;
                }
            }

            uint64_t total = 1;

            for (uint32_t i = 0; i < tnum; i++) {
                uint64_t result = 0;
                pthread_join(threads[i], (void **)&result);
                total = MultModulo(total, result, mod);
            }

            printf("Total: %lu\n", total);

            char buffer[sizeof(total)];
            memcpy(buffer, &total, sizeof(total));

            err = send(client_fd, buffer, sizeof(total), 0);
            if (err < 0) {
                fprintf(stderr, "Can't send data to client\n");
                break;
            }
        }

        shutdown(client_fd, SHUT_RDWR); 
        close(client_fd);
    }

    return 0;
}