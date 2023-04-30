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

//м'ютекс для синхронізації доступу до критичної секції коду
mutex mtx;

void multiply(int i, int j) {
    int result = 0;
    for (int t = 0; t < m; t++) {
        result += A[i][t] * B[t][j];
    }

    //блокуємо доступ до спільного ресурса - матриці С (якщо інший поток вже заблокував, то чекаємо поки розблокується)
    mtx.lock();
    C[i][j] = result;
    cout << "[" << i << "," << j << "]=" << result << endl;
    //розблоковуємо, щоб інші потоки мали доступ
    mtx.unlock();
}

//рандомним чином заповнюємо матриці А та В
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

//метод для виведення матриці в консоль
void print_matrix(const vector<vector<int>> &matrix, const string &name) {
    cout << "Matrix " << name << ":" << endl;
    for (const auto &row : matrix) {
        cout<<"|";
        for (int elem : row) {
            cout << elem << " ";
        }
        cout << "|" << endl;
    }
}

int main() {
    random_fill_matrix();

    print_matrix(A, "A");
    print_matrix(B, "B");

    vector<thread> threads; //створили вектор потоків, в наступному циклі n*k разів буде додаватись новий поток з виконанням множення матриць
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            threads.emplace_back(thread(multiply, i, j));
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    //після створення всіх потоків викликаємо join() для кожного потоку, щоб дочекатися завершення їх роботи
    for (auto &t : threads) {
        t.join();
    }

    print_matrix(C, "C");

    return 0;
}
