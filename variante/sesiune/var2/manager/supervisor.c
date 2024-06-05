#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

void parseaza_fisier(char* filename, int fd_fifo_sup_to_w1) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        exit(7);
    }

    while (1) {
        char buf[64] = { 0 };
        int line_len = 0;
        int bytes_read = 0;
        while ((bytes_read = read(fd, buf + line_len, sizeof(char))) > 0) {
            if (buf[line_len] == '\n') {
                buf[line_len] = 0;
                break;
            }
            line_len++;
        }
        if (bytes_read == 0 || bytes_read == -1) {
            break;
        }

        char* t1 = NULL, *t2 = NULL;
        char* p = strtok(buf, " ");
        while (p) {
            if (t1 == NULL) {
                t1 = p;
            }
            else {
                t2 = p;
            }
            p = strtok(NULL, " ");
        }

        int it1, it2;
        it1 = atoi(t1);
        it2 = atoi(t2);
        if (write(fd_fifo_sup_to_w1, &it1, sizeof(int)) == -1) {
            perror("Eroare la write");
            exit(9);
        }
        if (write(fd_fifo_sup_to_w1, &it2, sizeof(int)) == -1) {
            perror("Eroare la write");
            exit(10);
        }
    }
}
void citeste_res(int* w2_to_sup) {
    printf("%d x %d = %d\n",
            w2_to_sup[0],
            w2_to_sup[1],
            w2_to_sup[0] * w2_to_sup[1]);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./supervisor path");
        exit(1);
    }

    if (mkfifo("sup_to_w1_var2", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(2);
        }
    }
    int fd_fifo_sup_to_w1 = open("sup_to_w1_var2", O_WRONLY);
    if (fd_fifo_sup_to_w1 == -1) {
        perror("Eroare la open");
        exit(3);
    }

    int fd_shm = shm_open("w2_to_sup", O_RDWR | O_CREAT, 0600);
    if (fd_shm == -1) {
        perror("Eroare la shm_open");
        exit(4);
    }
    if (ftruncate(fd_shm, 2 * sizeof(int)) == -1) {
        perror("Eroare la ftruncare");
        exit(5);
    }
    int* w2_to_sup = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (w2_to_sup == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(6);
    }

    parseaza_fisier(argv[1], fd_fifo_sup_to_w1);
    close(fd_fifo_sup_to_w1);

    sleep(1);

    citeste_res(w2_to_sup);
    munmap(w2_to_sup, 2 * sizeof(int));
    return 0;
}
