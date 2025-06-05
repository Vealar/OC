//
// Created by Александр Артемьев on 16.03.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "Roll.c"
#define BUFFER_SIZE 5000

char bufferInput[BUFFER_SIZE + 1];
char bufferOutput[BUFFER_SIZE + 1];



int main(int argc, char* argv[]) {

    int bytesI,bytesO;

    if (argc != 3) {
        fprintf(stderr, "Please provide input and output file names.\n");
        return EXIT_FAILURE;
    }

    int input_fd = open(argv[1], O_RDONLY);

    if (input_fd == -1) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }
    char* name1 = "/tmp/channel";
    mkfifo(name1, 0666);

    int pipe1 = open(name1, O_RDWR | O_CREAT, 0666);
    if (pipe1 < 0) {
        perror("Error creating pipe1");
        return EXIT_FAILURE;
    }
    if ((bytesI = read(input_fd, bufferInput, BUFFER_SIZE)) == -1) {
        fprintf(stderr, "Cannot read from input file.\n");
        return EXIT_FAILURE;
    }
    if (write(pipe1, bufferInput, bytesI) == -1) {
        perror("Error writing to pipe1");
        return EXIT_FAILURE;
    }
    sleep(2);
    int output_fd = open(argv[2], O_RDWR | O_CREAT, 0666);
    if (output_fd == -1) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }
    if ((bytesO = read(pipe1, bufferOutput, BUFFER_SIZE)) == -1) {
        fprintf(stderr, "Can't read from fifo\n");
        return EXIT_FAILURE;
    }
    write(output_fd, bufferOutput, bytesO);
    close(output_fd);
    close(pipe1);
    unlink(name1);
    return EXIT_SUCCESS;
}