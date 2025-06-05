#include "Departments.h"
#include <thread>

int Departments::count_customers = 0;

Departments::Departments(int id_) : id(id_) {
    pthread_mutex_init(&mutex, nullptr);

}

bool Departments::isEmpty() {
    return queue.size() == 0;
};

void *Departments::threadDepartments(void *args) {
    auto department = reinterpret_cast<Departments *>(args);
    Customer *customer;
    // Цикл работает, пока кто то есть в магазине
    while (count_customers != 0) {
        if (!department->isEmpty()) {
            // Захват mutex для очереди
            pthread_mutex_lock(&department->mutex);
            // Взяли очередного покупателя
            customer = department->queue.front();
            // Удалили из очереди
            department->queue.pop();
            std::this_thread::sleep_for(std::chrono::nanoseconds(500000000));
            pthread_mutex_lock(&Customer::MUTEX);
            std::cout << "Отдел " << department->id + 1 << " рассчитал покупателя " << customer->id + 1 << "\n";
            *Customer::potok << "Отдел " << department->id + 1 << " рассчитал покупателя " << customer->id + 1 << "\n";
            pthread_mutex_unlock(&Customer::MUTEX);
            // Отпускаем покупателя
            {
                std::lock_guard<std::mutex> lock(customer->mutex_access);
                customer->commandReceived = true;
            }
            (customer->cv).notify_one();
            // Разблокировали очередь
            pthread_mutex_unlock(&department->mutex);
        }
    }
    return nullptr;
}


Departments::~Departments() {
    pthread_mutex_destroy(&mutex);
}

