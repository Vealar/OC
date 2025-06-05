#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <random>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/mman.h>
#include <queue>

struct SharedData {
    std::queue<int> queue;
    int N;
    int NUM_CUSTOMERS;
    int waiting = 0;
};
struct SharedData *shared_data = NULL;

sem_t *barbershop;
pid_t barbershop_pid;
sem_t *customer;

void sigint(int signal) {
    //sem_destroy(barbershop);
    //sem_destroy(customer);
    munmap(shared_data, sizeof(struct SharedData));
    shm_unlink("/barbershop_shared_memory");
    exit(0);
}

void Barber() {
    while (true) {
        sem_wait(customer);
        std::cout << "Barber is working "<< std::endl;
        //shared_data->queue.pop();
        shared_data->waiting--;
        sleep(1);
        std::cout << "Barber is sleeping" << std::endl;
        sem_post(barbershop);
    }
}

void Customer(int i) {
    usleep(rand() % 5000000 + 1000);
    std::cout << "Customer " << i << " in barbershop" << std::endl;
    if(shared_data->waiting==shared_data->N){
        std::cout<<"Customer is gone, because queue is full"<< std::endl;
        exit(0);
    }
    shared_data->waiting++;
    //shared_data->queue.push(i);
    std::cout << "Customer " << i << " is in queue " << std::endl;
    sem_post(customer);
    sem_wait(barbershop);
    sleep(1);
    std::cout << "Customer " << i << " is ready "<< std::endl;
    sleep(1);
    std::cout << "Customer " << i << " has left" << std::endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    int shared_memory_fd = shm_open("/barbershop_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shared_memory_fd, sizeof(struct SharedData));
    shared_data = (struct SharedData *) mmap(NULL, sizeof(struct SharedData), PROT_READ | PROT_WRITE,
                                             MAP_SHARED, shared_memory_fd, 0);
    close(shared_memory_fd);

    if (argc < 2) {
        std::cerr << "Input count of customers and count of chairs" << std::endl;
        return 1;
    }
    shared_data->NUM_CUSTOMERS = std::atoi(argv[1]);
    shared_data->N =  std::atoi(argv[2]);
    signal(SIGINT, sigint);

    // Создаем семафоры и процессы
    barbershop = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);;
    customer = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (barbershop == SEM_FAILED || customer == SEM_FAILED) {
        std::cerr << "Failed to create semaphore" << std::endl;
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {
        Barber();
        return 0;
    }else{
        barbershop_pid = pid;
    }
    //std::cout<<shared_data->NUM_CUSTOMERS;
    for (int i = 1; i <= shared_data->NUM_CUSTOMERS; ++i) {
        if (fork() == 0) {
            Customer(i);
            return 0;
        }
    }

    // Ждем завершения процессов
    for (int i = 0; i < shared_data->NUM_CUSTOMERS; ++i) {
        wait(NULL);
    }
    std::cout << "All customers have left the barbershop" << std::endl;
    kill(barbershop_pid,SIGINT);
    sigint(0);
    return 0;
}