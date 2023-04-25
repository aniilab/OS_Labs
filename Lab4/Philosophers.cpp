#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <locale.h>

using namespace std;

const int COUNT = 5;
mutex main_mutex;

class Fork {
public:
    Fork() : state(), owner(-1) {}

    bool pickUp(int id) {
        unique_lock<mutex> lock(state);
        int attempts = 0;
        while (owner != -1 && owner != id && attempts < COUNT) {
            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(rand() % 10));
            lock.lock();
            attempts++;
        }
        if (owner == -1) {
            owner = id;
            return true;
        }
        return false;
    }

    void putDown(int id) {
        std::unique_lock<mutex> lock(state);
        owner = -1;
    }

private:
    std::mutex state;
    int owner;
};


class Philosopher {
public:
    Philosopher(int id, Fork& left, Fork& right)
            : id(id), left_hand(left), right_hand(right) {}

    void operator()() {
        while (true) {
            think();

            bool pickedUp = false;
            while (!pickedUp) {
                pickedUp = left_hand.pickUp(id);
                if (!pickedUp) {
                    std::this_thread::sleep_for(chrono::milliseconds(rand() % 10));
                }
            }

            pickedUp = false;
            while (!pickedUp) {
                pickedUp = right_hand.pickUp(id);
                if (!pickedUp) {
                    left_hand.putDown(id);
                    this_thread::sleep_for(chrono::milliseconds(rand() % 10));
                }
            }

            eat();

            left_hand.putDown(id);
            right_hand.putDown(id);
        }
    }


private:
    void think() {
        cout << "Фiлософ " << id << " думає.\n";
        this_thread::sleep_for(chrono::milliseconds(1500));
    }
    void eat() {
        cout << "Фiлософ " << id << " їсть спагеттi.\n";
        this_thread::sleep_for(chrono::milliseconds(1500));
    }

    int id;
    Fork& left_hand;
    Fork& right_hand;
};

int main() {
    setlocale(LC_ALL, "Ukrainian");
    vector<Fork> forks(COUNT);
    vector<Philosopher> philosophers;
    for (int i = 0; i < COUNT; ++i) {
        philosophers.emplace_back(i, forks[i], forks[(i + 1) % COUNT]);
    }
    vector<thread> threads;
    for (auto& philosopher : philosophers) {
        threads.emplace_back(ref(philosopher));
    }

    // Блокуємо вхід до потоків
    {
        unique_lock<mutex> lock(main_mutex);
        // Дочікуємось завершення всіх потоків
        for (auto& thread : threads) {
            thread.join();
        }
    }

    return 0;
}

