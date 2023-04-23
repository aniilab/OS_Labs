#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main() {
    const char *shared_memory = "/shared_message";
    size_t shared_memory_size = 256;

    int fd = shm_open(shared_memory, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        cerr << "Error opening shared message." << endl;
        return 1;
    }
    if (ftruncate(fd, shared_memory_size) == -1) {
        cerr << "Error resizing shared message." << endl;
        return 1;
    }
    char *message = (char *)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    strncpy(message, "Hello world!", shared_memory_size);

    while (true) {
        cout << "Process ID: " << getpid() << endl;
        cout << "Message: " << message << endl;
        cout << "Enter new message: ";
        cin.getline(message, shared_memory_size);
        cout << endl;
    }

    munmap(message, shared_memory_size);
    shm_unlink(shared_memory);
    return 0;
}
