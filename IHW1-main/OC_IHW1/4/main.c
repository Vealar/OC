//
// Created by Александр Артемьев on 16.03.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unistd.h"
#include <fcntl.h>
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


    int pipe1[2];
    if (pipe(pipe1) != 0) {
        perror("Error creating pipe1");
        return EXIT_FAILURE;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error forking process");
        return EXIT_FAILURE;
    }

    if (pid1 == 0) {
        bytesI = read(input_fd, bufferInput, BUFFER_SIZE);
        if (write(pipe1[1], bufferInput, bytesI) == -1) {
            perror("Error writing to pipe1");
            return EXIT_FAILURE;
        }
        close(pipe1[1]);
        close(pipe1[0]); // Close the read end of pipe1 in child

        exit(EXIT_SUCCESS);
    } else {
        close(pipe1[1]); // Close the write end of pipe1 in parent
        wait(NULL);
    }
    close(input_fd);

    int pipe2[2];
    if (pipe(pipe2) != 0) {
        perror("Error creating pipe2");
        return EXIT_FAILURE;
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Error forking process");
        return EXIT_FAILURE;
    }

    if (pid2 == 0) {
        bytesR = read(pipe1[0], bufferRoll, BUFFER_SIZE);
        close(pipe1[0]); // Close the write end of pipe1 in child
        rollText(bytesR,bufferRoll);
        if (write(pipe2[1], bufferRoll, bytesR) == -1) {
            perror("Error writing to pipe2");
            return EXIT_FAILURE;
        }
        close(pipe2[1]);
        close(pipe2[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipe1[0]); // Close the read end of pipe1 in parent
        close(pipe2[1]); // Close the write end of pipe2 in parent
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
        bytesO = read(pipe2[0], bufferOutput, BUFFER_SIZE);
        if (write(output_fd, bufferOutput, bytesO) == -1) {
            perror("Error writing to output file");
            return EXIT_FAILURE;
        }
        close(pipe2[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipe2[0]); // Close the read end of pipe2 in parent
        wait(NULL);
    }

    close(output_fd);
    return EXIT_SUCCESS;
}