//
// Created by Александр Артемьев on 07.05.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    if (argc != 3) {
        printf("Введите ip и port, example: ./ClientPost 127.0.0.1 8080\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect() error");
        exit(1);
    }

    while (1) {
        printf("Введите сообщение: ");
        fgets(buffer, 1024, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        write(sock, buffer, strlen(buffer));

        if (strcmp(buffer, "The End") == 0) {
            break;
        }
    }

    close(sock);

    return 0;
}




