#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void gaseste_pereche_dif_max(int w1_to_w2, int* map) {
    int dif_max = 0;
    while (1) {
        int t1 = 0, t2 = 0;
        if (read(w1_to_w2, &t1, sizeof(int)) != sizeof(int)) {
            printf("w2 br1\n");
            break;
        }
        if (read(w1_to_w2, &t2, sizeof(int)) != sizeof(int)) {
            printf("w2 br2\n");
            break;
        }
        printf("w2: %d %d\n", t1, t2);

        int dif = abs(t1 - t2);
        if (dif > dif_max) {
            dif_max = dif;
            map[0] = t1;
            map[1] = t2;
        }
    }
}

int main(int argc, char* argv[]) {
    printf("w2 start\n");
    int w1_to_w2[2] = { 0 };
    if (pipe(w1_to_w2) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    pid_t w1 = fork();
    if (w1 == -1) {
        perror("Eroare la fork");
        exit(2);
    }
    else if (w1 == 0) {
        close(w1_to_w2[0]);
        if (dup2(w1_to_w2[1], 10) == -1) {
            perror("Eroare la dup2");
            exit(3);
        }
        close(w1_to_w2[0]);
        execl("workers/worker1", "worker1", NULL);
        exit(20);
    }

    close(w1_to_w2[1]);

    int w2_to_sup = shm_open("worker2_to_supervisor", O_RDWR | O_CREAT, 0600);
    if (w2_to_sup == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }
    if (ftruncate(w2_to_sup, 2 * sizeof(int)) == -1) {
        perror("Eroare la ftruncate");
        exit(2);
    }
    int* map = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, w2_to_sup, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(3);
    }

    printf("intru\n");
    gaseste_pereche_dif_max(w1_to_w2[0], map);

    close(w1_to_w2[0]);
    munmap(map, 2 * sizeof(int));
    close(w2_to_sup);
    return 0;
}
