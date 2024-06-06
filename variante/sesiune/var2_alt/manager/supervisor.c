#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void parseaza_fisier(char* filename, int sup_to_w1) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        exit(7);
    }
    while (1) {
        char buf[64] = { 0 };
        int buf_len = 0;
        ssize_t bytes_read = 0;
        printf("sup: citesc prima linie\n");
        while (1) {
            printf("sup: incep bucla\n");
            bytes_read = read(fd, &buf[buf_len], sizeof(char));
            if (bytes_read != sizeof(char)) {
                printf("sup: br1 %lu\n", bytes_read);
                break;
            }
            printf("sup: am citit caracterul %c\n", buf[buf_len]);
            if (buf[buf_len] == '\n') {
                buf[buf_len] = 0;
                printf("sup: br2\n");
                break;
            } 
            buf_len++;
        }
        if (bytes_read == -1 || bytes_read != 1) {
            printf("sup: br3\n");
            break;
        }
        printf("sup: prima linie este %s\n", buf);

        char* p = strtok(buf, " ");
        int t1 = -1, t2 = -1;
        while (p) {
            if (t1 == -1) {
                t1 = atoi(p);
            }
            else {
                t2 = atoi(p);
            }
            p = strtok(NULL, " ");
        }

        printf("sup: scriu %d,%d prin fifo write pentru w1\n", t1, t2);
        if (write(sup_to_w1, &t1, sizeof(int)) == -1) {
            perror("Eroare la write");
            exit(8);
        }
        if (write(sup_to_w1, &t2, sizeof(int)) == -1) {
            perror("Eroare la write");
            exit(9);
        }
    }
    close(fd);
}

void afiseaza_rezultat(int* map) {
    printf("res %d x %d = %d\n", map[0], map[1], map[0] * map[1]);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Numar invalid de argumente");
        exit(1);
    }

    if (mkfifo("supervisor_to_worker1", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(2);
        }
    }
    int sup_to_w1 = open("supervisor_to_worker1", O_WRONLY);
    if (sup_to_w1 == -1) {
        perror("Eroare la open");
        exit(3);
    }
    printf("sup: am deschis fifo write pentru w1\n");

    int w2_to_sup = shm_open("worker2_to_supervisor", O_RDWR | O_CREAT, 0600);
    if (w2_to_sup == -1) {
        perror("Eroare la shm_open");
        exit(4);
    }
    if (ftruncate(w2_to_sup, 2 * sizeof(int)) == -1) {
        perror("Eroare la ftruncate");
        exit(5);
    }
    int* map = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, w2_to_sup, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(6);
    }

    parseaza_fisier(argv[1], sup_to_w1);

    sleep(1);

    afiseaza_rezultat(map);

    munmap(map, 2 * sizeof(int));
    close(w2_to_sup);
    close(sup_to_w1);
    return 0;
}
