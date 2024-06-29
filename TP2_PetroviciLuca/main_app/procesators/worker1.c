#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int sum_digits = 0;
int n = 0;

void proceseaza_de_la_sup(char* shm_ptr, int fifo) {
    memcpy(&n, shm_ptr, sizeof(int)); // TODO: aici ar tb sa fie un sync
    while (!n) {
        sleep(1);
        memcpy(&n, shm_ptr, sizeof(int)); // TODO: aici ar tb sa fie un sync
        printf("w1 sleeping\n");
    }
    printf("")
    for (int i = 4; i < n + 4; i++) {
        char ch = shm_ptr[i];
        if (isdigit(ch)) {
            sum_digits += (int)(ch - '0');
        }
        else {
            if (write(fifo, &ch, sizeof(char)) == -1) {
                perror("Eroare la write w1");
                exit(10); // nu sunt sigur daca trebuie chiar sa dau exit
            }
        }
    }
}

void proceseaza_de_la_w2(int pipe_fd, char* shm_ptr) {
    int suma_ascii = 0;
    for (int i = 0; i < n; i++) {
        char ch = 0; 
        if (read(pipe_fd, &ch, sizeof(char)) == -1) {
            perror("Eroare la read");
            exit(11);
        }

        suma_ascii += (int)(ch);
    }
    int op = 0;
    if (read(pipe_fd, &op, sizeof(int)) == -1) {
        perror("Eroare la read");
        exit(12);
    }
    suma_ascii -= op;

    memcpy(shm_ptr, &suma_ascii, sizeof(int));
    memcpy(shm_ptr, &sum_digits, sizeof(int));
}

int main(int argc, char* argv[]) {
    int shm_fd = shm_open("sup_and_w1", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (shm_fd == -1) {
        perror("Eroare la shm_open");
        exit(9);
    }
    if (ftruncate(shm_fd, 4096) == -1) {
        perror("Eroare la ftruncate");
        exit(10);
    }
    char* shm_ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(2);
    }

    if (mkfifo("w1_and_w2", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(4);
        }
    }
    int w1_and_w2 = open("w1_and_w2", O_RDONLY);
    if (w1_and_w2 == -1) {
        perror("Eroare la open");
        exit(5);
    }

    proceseaza_de_la_sup(shm_ptr, w1_and_w2);

    proceseaza_de_la_w2(10, shm_ptr); // ca asa am decis eu

    close(w1_and_w2);

    munmap("sup_to_w1", 4096);
    shm_unlink("sup_to_w1");
    close(shm_fd);
    close(10);

    return 0;
}
