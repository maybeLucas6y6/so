#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void parseaza_fisier(const char* filename, int fifo) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        exit(5);
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

        char* t1 = NULL, *t2 = NULL, *op = NULL;
        char* p = strtok(buf, " ");
        while (p) {
            if (t1 == NULL) {
                t1 = p;
            }
            else if (op == NULL) {
                op = p; 
            }
            else {
                t2 = p;
            }
            p = strtok(NULL, " ");
        }

        int it1, it2;
        it1 = atoi(t1);
        it2 = atoi(t2);
        write(fifo, &it1, sizeof(int));
        write(fifo, op, sizeof(char));
        write(fifo, &it2, sizeof(int));
    }
}

void printeaza_date(int* min_max) {
    printf("min %d max %d sum %d", min_max[0], min_max[1], min_max[0] + min_max[1]);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./supervisor path");
        exit(1);
    }

    if (mkfifo("sup_to_w1", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Aoleu");
            exit(2);
        }
    }

    int fd_w_fifo = open("sup_to_w1", O_WRONLY);
    if (fd_w_fifo == -1) {
        perror("Eroare la open");
        exit(3);
    }

    // ocreat? rdwr?
    int fd_r_shmap = shm_open("w2_to_sup", O_RDWR | O_CREAT, 0600);
    if (fd_r_shmap == -1) {
        perror("Eroare la open");
        exit(4);
    }
    // protwrite?
    int* min_max = mmap(NULL, 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd_r_shmap, 0);
    if (min_max == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(5);
    }

    if (ftruncate(fd_r_shmap, sizeof(int) * 2) == -1) {
        perror("Eroare la ftruncate");
        exit(6);
    }

    parseaza_fisier(argv[1], fd_w_fifo);
    close(fd_w_fifo);

    sleep(1);

    printeaza_date(min_max);
    close(fd_r_shmap);
    munmap(min_max, 8);

    return 0;
}
