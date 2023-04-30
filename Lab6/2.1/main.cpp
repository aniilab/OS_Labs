#include <iostream>
#include <thread>
#include <mutex>
#include <cmath>

using namespace std;

const int iterations = 1000000000;

//з використанням критичного сегменту
void with_mutex(int &v, mutex &mtx) {
    for (int i = 0; i < iterations; i++) {
        mtx.lock();
        v++;
        mtx.unlock();
    }
}

//без використання критичного сегменту
void without_mutex(int &v) {
    for (int i = 0; i < iterations; i++) {
        v++;
    }
}

int main() {
    int v1 = 0;
    mutex mtx;

    //З використанням критичного сегменту
    auto start_time1 = chrono::high_resolution_clock::now();
    thread t1(with_mutex, ref(v1), ref(mtx));
    thread t2(with_mutex, ref(v1), ref(mtx));
    t1.join();
    t2.join();
    auto end_time1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_time1 = end_time1 - start_time1;
    cout << "With critical section:" << endl;
    cout << "v1 = " << v1 << "; time: " << elapsed_time1.count() << endl;
    cout << endl;

    //Без використання критичного сегменту
    int v2 = 0;
    auto start_time2 = chrono::high_resolution_clock::now();
    thread t3(without_mutex, ref(v2));
    thread t4(without_mutex, ref(v2));
    t3.join();
    t4.join();
    auto end_time2 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_time2 = end_time2 - start_time2;
    cout << "Without critical section:" << endl;
    cout << "v2 = " << v2 << "; time: " << elapsed_time2.count() << endl;
    cout << endl;

    cout << "Difference: |v1-v2| = " << abs(v1 - v2) << endl;
    cout << "Time difference: = " << elapsed_time1.count() - elapsed_time2.count();

    return 0;
}
