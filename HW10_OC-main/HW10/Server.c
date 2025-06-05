//
// Created by Александр Артемьев on 05.05.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
#define THE_END "The End"

int main(int argc, char *argv[])
{
    // Сокеты для взаимодействия с двумя клиентами
    int server_socket;
    int getter_socket;
    int post_socket;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    socklen_t clnt_addr_size;
    char buffer[BUF_SIZE];
    int str_len;

    if (argc != 2) {
        printf("Для корректного запуска программы введите \"./Server {number of port}\" ,example"
               "./Server 8080 \n", argv[0]);
        exit(1);
    }

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка socket()");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Ошибка bind()");
        exit(1);
    }

    if (listen(server_socket, 2) == -1) {
        perror("Ошибка listen()");
        exit(1);
    }

    clnt_addr_size = sizeof(client_address);
    post_socket = accept(server_socket, (struct sockaddr *)&client_address, &clnt_addr_size);
    getter_socket = accept(server_socket, (struct sockaddr *)&client_address, &clnt_addr_size);

    while (1) {
        str_len = read(post_socket, buffer, BUF_SIZE);
        buffer[str_len] = '\0';
        write(getter_socket, buffer, str_len);

        if (strcmp(buffer, THE_END) == 0) {
            break;
        }
    }

    close(post_socket);
    close(getter_socket);
    close(server_socket);

    return 0;
}

