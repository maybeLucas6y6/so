#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void selectare_pereche(int w1_to_w2, int* w2_to_sup) {
    int dif_max = 0;
    while (1) {
        int t1 = 0, t2 = 0;
        int bytes_read = 0;
        bytes_read = read(w1_to_w2, &t1, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(8);
        }
        if (bytes_read != sizeof(int)) {
            break;
        }

        bytes_read = read(w1_to_w2, &t2, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(9);
        }
        if (bytes_read != sizeof(int)) {
            break;
        }

        int dif = abs(t1 - t2);
        if (dif > dif_max) {
            dif_max = dif;
            w2_to_sup[0] = t1;
            w2_to_sup[1] = t2;
        }
    }
}

int main(int argc, char* argv[]) {
    int w1_to_w2[2];
    if (pipe(w1_to_w2) == -1) {
        perror("Eroare la pipe");
        exit(3);
    }

    pid_t pid_w1 = fork();
    if (pid_w1 == -1) {
        perror("Eroare la fork");
        exit(4);
    }
    else if (pid_w1 == 0) {
        close(w1_to_w2[0]);
        if (dup2(w1_to_w2[1], STDOUT_FILENO) == -1) {
            perror("Eroare la dup2");
            exit(5);
        }
        close(w1_to_w2[1]);
        execl("workers/worker1", "worker1", NULL);
        perror("Eroare la exec");
        exit(69);
    }

    close(w1_to_w2[1]);
    int fd_shm = shm_open("w2_to_sup", O_RDWR | O_CREAT, 0600);
    if (fd_shm == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }
    if (ftruncate(fd_shm, 2 * sizeof(int)) == -1) {
        perror("Eroare la ftruncare");
        exit(7);
    }
    int* w2_to_sup = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (w2_to_sup == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(2);
    }

    selectare_pereche(w1_to_w2[0], w2_to_sup);
    
    close(w1_to_w2[0]);
    munmap(w2_to_sup, 2 * sizeof(int));
    return 0;
}
