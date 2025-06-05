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
char bufferRoll[BUFFER_SIZE + 1];
char bufferOutput[BUFFER_SIZE + 1];



int main(int argc, char* argv[]) {

    int bytesI, bytesR, bytesO;

    if (argc != 3) {
        fprintf(stderr, "Please provide input and output file names.\n");
        return EXIT_FAILURE;
    }

    int input_fd = open(argv[1], O_RDONLY);

    if (input_fd == -1) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    char* name1 = "/tmp/input_channel";
    mkfifo(name1, 0666);

    int pipe1 = open(name1, O_RDWR | O_CREAT);
    if (pipe1 < 0) {
        perror("Error opening input_channel");
        return EXIT_FAILURE;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error forking process");
        return EXIT_FAILURE;
    }

    if (pid1 == 0) {
        bytesI = read(input_fd, bufferInput, BUFFER_SIZE);
        if (bytesI== 0) {
            close(pipe1);
            close(input_fd);
            unlink(name1);
            return EXIT_SUCCESS;
        }
        if (write(pipe1, bufferInput, bytesI) == -1) {
            perror("Error writing to pipe1");
            return EXIT_FAILURE;
        }
        close(pipe1);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }

    close(input_fd);

    char* name2 = "/tmp/roll_channel";
    mkfifo(name2, 0666);

    int pipe2 = open(name2, O_RDWR | O_CREAT);
    if (pipe2 < 0) {
        perror("Error creating pipe2");
        return EXIT_FAILURE;
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Error forking process");
        return EXIT_FAILURE;
    }

    if (pid2 == 0) {
        bytesR = read(pipe1, bufferRoll, BUFFER_SIZE);
        rollText(bytesR,bufferRoll);
        if (write(pipe2, bufferRoll, bytesR) == -1) {
            perror("Error writing to pipe2");
            return EXIT_FAILURE;
        }
        close(pipe2);
        close(pipe1);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }

    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (output_fd == -1) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    pid_t pid3 = fork();
    if (pid3 == -1) {
        perror("Error forking process");
        return EXIT_FAILURE;
    }

    if (pid3 == 0) {
        bytesO = read(pipe2, bufferOutput, BUFFER_SIZE);
        if (write(output_fd, bufferOutput, bytesO) == -1) {
            perror("Error writing to output file");
            return EXIT_FAILURE;
        }
        close(pipe1);
        close(pipe2);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }
    close(pipe1);
    close(pipe2);

    unlink(name1);
    unlink(name2);
    close(output_fd);
    return EXIT_SUCCESS;
}