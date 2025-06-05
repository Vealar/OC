#include <queue>
#include "Customer.h"

class Departments {
public:
    int id;
    // Счетчик покупателей, которые есть в магазине
    static int count_customers;
    // Очередь для этого отделения
    std::queue<Customer*> queue;
    // На каждую очередь свой mutex
    pthread_mutex_t mutex;
    // Конструктор для создания отделения
    Departments(int id_);
    ~Departments();
    // Работа очереди
    static void* threadDepartments(void* args);
    // Возвращает пуста ли очередь
    bool isEmpty();
};

