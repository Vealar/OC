#include "Departments.h"
#include <thread>

// Количество отделов
#define COUNT_DEPARTMENTS 3

// Вектор, который содержит все отделения
std::vector<Departments*> departments(COUNT_DEPARTMENTS);

void ExitThread(std::vector<pthread_t> pthreads) {
    for(int i =0;i<pthreads.size();++i){
        pthread_join(pthreads[i], nullptr);
    }
}
bool is_number(const std::string& string)
{
    return !string.empty() && (string.find_first_not_of("0123456789") == string.npos);
}
// Покупатель встает в очередь и приобретает товар
static void* threadCustomer(void *args){
    auto customer = reinterpret_cast<Customer*>(args);
    int id_department;
    // Покупатель проходит по каждому отделу
    for (int i = 0; i < customer->order.size(); ++i) {
        customer->commandReceived = false;
        // Номер текущего отдела
        id_department = customer->order[i];
        // Захватываем mutex для этой очереди
        pthread_mutex_lock(&departments[id_department - 1]->mutex);
        pthread_mutex_lock(&Customer::MUTEX);
        std::cout << "Покупатель " << customer->id + 1 << " встал в очередь к продавцу " << id_department << "\n";
        *Customer::potok<<"Покупатель " << customer->id + 1 << " встал в очередь к продавцу " << id_department << "\n";
        pthread_mutex_unlock(&Customer::MUTEX);
        departments[id_department - 1]->queue.push(customer);
        //mutex освобожден
        pthread_mutex_unlock(&departments[id_department - 1]->mutex);
        // Поток ожидает ответа от продавца, то есть когда очерердь дойдет до него
        std::unique_lock<std::mutex> lock(customer->mutex_access);
        while (!customer->commandReceived) {
            (customer->cv).wait(lock); // поток блокируется в ожидании уведомления
        }

    }
    Departments::count_customers -= 1;
    return nullptr;
}
int main(int num, char* arguments[]) {
    Customer::FullMutex();
    std::string input = arguments[1];
    std::string current;

    if(is_number(input)){
        Departments::count_customers = std::stoi(input);
        current = arguments[2];
    }else{
        auto input_threads = std::ifstream(input);
        input_threads>>current;
        Departments::count_customers = std::stoi(current);
        input_threads>>current;
    }
    std::ofstream output_thread;
    output_thread.open(current);
    Customer::potok = &output_thread;
    //std::cout<<"dsad";
    //std::cin>> Departments::count_customers;
    // Поток для каждого покупателя
    std::vector<pthread_t> threadsC(Departments::count_customers);
    // Поток для каждого отдела
    std::vector<pthread_t> threadsD(COUNT_DEPARTMENTS);
    std::string filename;

    for (int i = 0; i < COUNT_DEPARTMENTS; ++i) {
        departments[i] = new Departments(i);
    }
    // Создание потоков отделов
    for (int i = 0; i < COUNT_DEPARTMENTS; ++i) {
        pthread_create(&threadsD[i], nullptr,Departments::threadDepartments , (void *) departments[i]);
    }


    std::vector<Customer *> customers(Departments::count_customers);
    for (int i = 0; i < Departments::count_customers; ++i) {
        customers[i] = new Customer(i);
    }

    // Создание потоков покупателей
    for (int i = 0; i < Departments::count_customers; ++i) {
        pthread_create(&threadsC[i], nullptr, threadCustomer, (void *)customers[i]);
    }

    ExitThread(threadsC);
    ExitThread(threadsD);
    output_thread.close();

}
