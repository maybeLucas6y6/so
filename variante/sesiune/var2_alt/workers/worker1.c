#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void filtreaza_perechi_coprime(int sup_to_w1) {
    while (1) {
        int t1 = 0, t2 = 0;
        if (read(sup_to_w1, &t1, sizeof(int)) != sizeof(int)) {
            printf("w1 br1\n");
            break;
        }
        if (read(sup_to_w1, &t2, sizeof(int)) != sizeof(int)) {
            printf("w1 br2\n");
            break;
        }
        printf("w1: %d %d\n", t1, t2);

        int a = (t1 > t2) ? t1 : t2;
        int b = (t1 > t2) ? t2 : t1;
        while (a != b) {
            if (a > b) {
                a = a - b;
            }
            else {
                b = b - a;
            }
        }

        if (a == 1) {
            printf("w1: o sa scriu la w2 perechea %d,%d\n", t1, t2);
            if (write(10, &t1, sizeof(int)) != sizeof(int)) {
                break;
            }
            if (write(10, &t2, sizeof(int)) != sizeof(int)) {
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    printf("w1 start\n");
    if (mkfifo("supervisor_to_worker1", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(1);
        }
    }
    int sup_to_w1 = open("supervisor_to_worker1", O_RDONLY);
    if (sup_to_w1 == -1) {
        perror("Eroare la open");
        exit(2);
    }
    printf("w1: am deschis fifo read pentru sup\n");

    filtreaza_perechi_coprime(sup_to_w1);

    close(sup_to_w1);
    return 0;
}
