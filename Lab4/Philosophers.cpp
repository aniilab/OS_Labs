#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <locale.h>

using namespace std;

const int COUNT = 5;

class Fork {
public:
    Fork() : state() {}
    void pickUp() {
        state.lock();
    }
    void putDown() {
        state.unlock();
    }
private:
    mutex state;
};

class Philosopher {
public:
    Philosopher(int id, Fork& left, Fork& right)
        : id(id), left_hand(left), right_hand(right) {}

    void operator()() {
        while (true) {
            think();

            // Щоб не було взаємоблокування: парні (2-ий, 4-ий філософи) спочатку беруть праву виделку
            if (id % 2 == 0) {
                right_hand.pickUp();
                left_hand.pickUp();
            }
            // А непарні (1-ий, 3-ий, 5-ий філософи) беруть ліву виделку
            else {
                left_hand.pickUp();
                right_hand.pickUp();
            }
            eat();

            left_hand.putDown();
            right_hand.putDown();
        }
    }

private:
    void think() {
        cout << "Фiлософ " << id << " думає.\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    void eat() {
        cout << "Фiлософ " << id << " їсть спагеттi.\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
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
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}