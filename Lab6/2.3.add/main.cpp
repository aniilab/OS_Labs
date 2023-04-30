#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

const int iterations = 1000;

mutex mtx;
condition_variable cv;
int v = 0;
bool first_thread_turn = true;

void synchronized(int thread_id) {
    unique_lock<mutex> lock(mtx);

    for (int i = 0; i < iterations; i++) {
        while ((thread_id == 1 && !first_thread_turn) || (thread_id == 2 && first_thread_turn)) {
            cv.wait(lock);
        }

        v++;
        cout << "Thread " << thread_id << ": " << v << endl;

        first_thread_turn = !first_thread_turn;
        cv.notify_all();
    }
}

int main() {
    thread t1(synchronized, 1);
    thread t2(synchronized, 2);

    t1.join();
    t2.join();

    cout << "Final value of v: " << v << endl;
    return 0;
}
