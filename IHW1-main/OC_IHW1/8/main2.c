//
// Created by Александр Артемьев on 16.03.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "Roll.c"
#define BUFFER_SIZE 5000
char bufferRoll[BUFFER_SIZE + 1];
int main(int argc, char* argv[]) {
    int bytesR;

    char* name1 = "/tmp/channel";
    sleep(1);
    mkfifo(name1, 0666);

    int pipe1 = open(name1, O_RDWR | O_CREAT, 0666);
    if (pipe1 < 0) {
        perror("Error creating pipe1");
        return EXIT_FAILURE;
    }
    if ((bytesR = read(pipe1, bufferRoll, BUFFER_SIZE)) == -1 || strcmp(bufferRoll, "exit") == 1) {
        fprintf(stderr, "Can't read fifo\n");
        close(pipe1);
        unlink(name1);
        return EXIT_FAILURE;
    }
    rollText(bytesR,bufferRoll);
    if (write(pipe1, bufferRoll, bytesR) == -1) {
        fprintf(stderr, "Can't write fifo\n");
        return EXIT_FAILURE;
    }
    close(pipe1);
    unlink(name1);
    return EXIT_SUCCESS;
}