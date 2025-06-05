#include "Customer.h"
#include "random"
std::ofstream* Customer::potok = nullptr;
std::vector<int> Customer::id_departments = {1, 2, 3};
pthread_mutex_t Customer::MUTEX = pthread_mutex_t();

void Customer::FullMutex() {
    pthread_mutex_init(&MUTEX, nullptr);
}

int Customer::random(int min, int max) {
    return min + std::rand() % (max - min + 1);
}

Customer::Customer(int id_) : id(id_) {
    int count_departments = random(1, id_departments.size());
    order = std::vector<int>(id_departments);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // Перемешиваем массив отделов
    std::shuffle(order.begin(), order.end(), std::default_random_engine(seed));
    // Оставляем только count_departments элементов
    order.erase(order.begin() + count_departments, order.end());
}

Customer::~Customer() {
}
