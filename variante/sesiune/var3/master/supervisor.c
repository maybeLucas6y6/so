#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void parseaza_fisier(char* filename, int sup_to_w1) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        exit(7);
    }
    while (1) {
        char c = 0;
        if (read(fd, &c, sizeof(char)) <= 0) {
            break;
        }

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            if (write(sup_to_w1, &c, sizeof(char)) != 1) {
                break;
            }
        }
    }
}

void citeste_rezultate(int* map) {
    // verif 0?
    printf("voc: %d, cons: %d\n", map[0], map[1]);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Numar invalid de argumente");
        exit(1);
    }

    int w2_to_sup = shm_open("comunicare_decriptie", O_RDWR | O_CREAT, 0600);
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

    int sup_to_w1[2] = { 0 };
    if (pipe(sup_to_w1) == -1) {
        perror("Eroare la pipe");
        exit(4);
    }

    pid_t w1 = fork();
    if (w1 == -1) {
        perror("Eroare la fork");
        exit(5);
    }
    else if (w1 == 0) {
        close(sup_to_w1[1]);
        if (dup2(sup_to_w1[0], STDIN_FILENO)) {
            perror("Eroare la dup2");
            exit(6);
        }
        close(sup_to_w1[0]);
        execl("slaves/worker1", "worker1", NULL);
        exit(20);
    }

    close(sup_to_w1[0]);

    parseaza_fisier(argv[1], sup_to_w1[1]);
    close(sup_to_w1[1]);

    sleep(2);
    citeste_rezultate(map);

    munmap(map, 2 * sizeof(int));
    close(w2_to_sup);
    return 0;
}
