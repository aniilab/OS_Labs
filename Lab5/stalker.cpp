#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    const char *shared_memory = "/shared_message";
    size_t shared_memory_size = 256;

    int fd = shm_open(shared_memory, O_RDONLY, S_IRUSR);
    if (fd == -1) {
        cerr << "Error opening shared memory." << endl;
        return 1;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        cerr << "Error getting shared memory size." << endl;
        return 1;
    }
    char *message = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    string current_message, previous_message;
    previous_message = string(message);

    cout << "I am your stalker and I am going to stalk all your actions ( •̀ᴗ•́ )و ̑̑" << endl;
    while (true) {
        current_message = string(message);

        if (current_message != previous_message) {
            cout << "Process ID: " << getpid() << endl;
            cout << "Message value has been changed to: " << current_message << endl;
            previous_message = current_message;
        }

        sleep(1);
    }

    munmap(message, shared_memory_size);
    close(fd);
    return 0;
}
