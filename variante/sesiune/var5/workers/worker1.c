#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int op = 0;

void read_from_sup_pass_to_w2(int* map) {
    int n = 0;
    map[0] = 0;
    while (1) {
        ssize_t bytes_read = 0;
        int num = 0;
        bytes_read = read(10, &num, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read w1 read_from_sup");
            exit(5);
        }
        if (bytes_read == 0) {
            break;
        }
        printf("w1 num %d\n", num);

        if (num % 2 == 1) {
            num *= 2;
            op++;
        }

        n++;
        map[n] = num;
    }
    //if (close(10) == -1) {
    //    perror("Eroare la close");
    //    exit(1);
    //}
    printf("w1 n %d\n", n);
    map[0] = n;
}

void read_from_w1_send_to_sup(int* map, int w1_to_sup) {
    int n = map[0];
    int sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += map[i]; 
    }
    if (write(w1_to_sup, &sum, sizeof(int)) == -1) {
        perror("Eroare la write");
        exit(7);
    }
    if (write(w1_to_sup, &op, sizeof(int)) == -1) {
        perror("Eroare la write");
        exit(8);
    }
}

int main(int argc, char* argv[]) {
    printf("w1 start\n");
    if (mkfifo("w1_to_sup", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(5);
        }
    }
    int w1_to_sup = open("w1_to_sup", O_WRONLY);
    if (w1_to_sup == -1) {
        perror("Eroare la open");
        exit(6);
    }

    int fd_map = shm_open("w1_and_w2", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd_map == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }
    if (ftruncate(fd_map, 4096) == -1) {
        perror("Eroare la ftruncate");
        exit(2);
    }
    int* map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd_map, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(3);
    }

    read_from_sup_pass_to_w2(map);
    read_from_w1_send_to_sup(map, w1_to_sup);

    if (munmap(map, 4096) == -1) {
        perror("Eroare la munmap");
        exit(4);
    }
    shm_unlink("w1_and_w2");
    close(fd_map);
    close(w1_to_sup);
    printf("w1 exit\n");
    return 0;
}
