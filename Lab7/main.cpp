#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <atomic>
using namespace std;

atomic<bool> stop_checking(false);

int f(int x) {
    if (x >= 50) {
        return 0;
    }
    while (true) {
    }
}

int g(int x) {
    if (x<=20) {
        return 0;
    }
    while (true) {
    }
}

void check_finished(pid_t pid_f, pid_t pid_g) {
    int status_f, status_g;
    pid_t finished_f = waitpid(pid_f, &status_f, WNOHANG);
    pid_t finished_g = waitpid(pid_g, &status_g, WNOHANG);

    if (finished_f || finished_g) {
        if ((finished_f && WEXITSTATUS(status_f) == 0) || (finished_g && WEXITSTATUS(status_g) == 0)) {
            cout << "Результат f(x) && g(x) = 0" << endl;
            kill(pid_f, SIGTERM);
            kill(pid_g, SIGTERM);
            stop_checking = true;
            exit(0);
        }
    }
}

void alarm_handler(int signum) {
    if (stop_checking) {
        return;
    }

    cout << "Обчислення не завершені. Виберіть опцію:" << endl
              << "1) Продовжити обчислення" << endl
              << "2) Припинити" << endl
              << "3) Продовжити, не перепитуючи більше" << endl;

    int option;
    cin >> option;

    if (option == 1) {
        alarm(10);
    } else if (option == 2) {
        kill(0, SIGTERM);
        exit(1);
    } else {
        stop_checking = true;
    }
}

int main() {
    signal(SIGALRM, alarm_handler);

    int x;
    cout << "Введіть значення x: ";
    cin >> x;

    pid_t f_pid = fork();
    if (f_pid == 0) {
        int result = f(x);
        cout << result;
        exit(0);
    }

    pid_t g_pid = fork();
    if (g_pid == 0) {
        int result = g(x);
        cout << result;
        exit(0);
    }

    alarm(10);

    while (!stop_checking) {
        check_finished(f_pid, g_pid);
        usleep(100000); // sleep for 100 ms
    }

    return 0;
}
