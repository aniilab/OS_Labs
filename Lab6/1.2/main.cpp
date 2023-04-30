#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

const int n = 5;
const int m = 3;
const int k = 7;

vector<vector<int>> A(n, vector<int>(m));
vector<vector<int>> B(m, vector<int>(k));
vector<vector<int>> C(n, vector<int>(k));

mutex mtx;

void multiply(int i, int j) {
    int result = 0;
    for (int t = 0; t < m; t++) {
        result += A[i][t] * B[t][j];
    }
    mtx.lock();
    C[i][j] = result;
    mtx.unlock();
}

void random_fill_matrix() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            A[i][j] = dis(gen);
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            B[i][j] = dis(gen);
        }
    }
}

void print_matrix(const vector<vector<int>> &matrix, const string &name) {
    cout << "Matrix " << name << ":" << endl;
    for (const auto &row : matrix) {
        for (int elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

double matrix_multiplication(int num_threads) {
    vector<thread> threads;
    int total_elements = n * k;
    int elements_per_thread = total_elements / num_threads;

    auto start_time = chrono::high_resolution_clock::now();

    for (int t = 0; t < num_threads; t++) {
        //створення нового потоку та додавання його до вектора потоків
        threads.emplace_back([&, t]() {
            //визначення діапазону елементів матриці C які будуть обчислені цим потоком
            int start = t * elements_per_thread;
            int end = (t == num_threads - 1) ? total_elements : (t + 1) * elements_per_thread;

            //цикл для обчислення кожного елемента матриці C, відповідного поточному потоку
            for (int e = start; e < end; e++) {
                //визначення індексів рядка та стовпця в матриці C для поточного елемента
                int i = e / k;
                int j = e % k;

                //обчислення поточного елемента матриці C, використовуючи функцію множення
                multiply(i, j);
            }
        });
    }

    for (auto &t: threads) {
        t.join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_time = end_time - start_time;

    return elapsed_time.count(); // повертаємо час виконання в секундах
}

int main() {
    random_fill_matrix();
    print_matrix(A, "A");
    print_matrix(B, "B");
    int max_threads = n * k;
    double min_time = numeric_limits<double>::max();
    int optimal_threads = 1;

// проводимо експеримент для кожної кількості потоків
    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {
        double elapsed_time = matrix_multiplication(num_threads);
        cout << "Time for " << num_threads << " threads: " << elapsed_time << " seconds." << endl;

        // зберігаємо кількість потоків з найшвидшим часом множення матриць
        if (elapsed_time < min_time) {
            min_time = elapsed_time;
            optimal_threads = num_threads;
        }
    }

    cout << "Optimal number of threads: " << optimal_threads << endl;
    cout << "Minimum time: " << min_time << " seconds." << endl;

    return 0;
}