#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

const int iterations = 1000000000;

//з використанням атомарної операції
void with_atomic(atomic<int> &v) {
    for (int i = 0; i < iterations; i++) {
        v.fetch_add(1);
    }
}

int main() {
    atomic<int> v = 0;
    auto start = chrono::high_resolution_clock::now();
    thread t1(with_atomic, ref(v));
    thread t2(with_atomic, ref(v));
    t1.join();
    t2.join();
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "With atomic operations: shared_var = " << v << "; time: " << elapsed.count() << "s" << endl;

    return 0;

}
