#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include <unistd.h>

void parseaza_perechi_coprime(int fd_fifo_sup_to_w1) {
    while (1) {
        int t1 = 0, t2 = 0;
        int bytes_read = 0;
        bytes_read = read(fd_fifo_sup_to_w1, &t1, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(3);
        }
        if (bytes_read != sizeof(int)) {
            break;
        }

        bytes_read = read(fd_fifo_sup_to_w1, &t2, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(4);
        }
        if (bytes_read != sizeof(int)) {
            break;
        }

        int a = MAX(t1, t2), b = MIN(t1, t2);
        while (a != b) {
            if (a > b) {
                a = a - b;
            }
            else {
                b = b - a;
            }
        }

        if (a == 1) {
            if (write(STDOUT_FILENO, &t1, sizeof(int) == -1)) {
                perror("Eroare la write");
                exit(5);
            }
            if (write(STDOUT_FILENO, &t2, sizeof(int) == -1)) {
                perror("Eroare la write");
                exit(6);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (mkfifo("sup_to_w1", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(1);
        }

    }
    int fd_fifo_sup_to_w1 = open("sup_to_w1", O_RDONLY);
    if (fd_fifo_sup_to_w1 == -1) {
        perror("Eroare la open worker1 fifo");
        exit(2);
    }

    parseaza_perechi_coprime(fd_fifo_sup_to_w1);

    close(fd_fifo_sup_to_w1);
    close(STDOUT_FILENO); // necesar???
    return 0;
}
