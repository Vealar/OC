#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

class Customer {
public:
    // Поле для потока записи в файл
    static std::ofstream* potok;
    // Поле для удержания консоли
    static pthread_mutex_t MUTEX;
    // Рандомное число от min до max
    static int random(int min, int max);
    // Вектор который содержит все номера отделов
    static std::vector<int> id_departments;
    // Заполняет статическое поле MUTEX
    static void FullMutex();
    // Номер покупателя, по которому его можно иденфицировать
    int id;
    // Порядок в котором он хочет обойти отделы
    std::vector<int> order;
    // Поля для ожидания покупателя, пока он стоит в очереди
    std::mutex mutex_access;
    std::condition_variable cv;
    bool commandReceived = false;
    // Конструктор в котором заполняется поле id и генерируется order
    Customer(int id);
    ~Customer();
};

