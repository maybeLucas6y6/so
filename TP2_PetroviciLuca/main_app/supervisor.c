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

int este_alnum(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9');
}

void parseaza_fisier(char* filename, char* shm_ptr) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        exit(3);
    }

    int n = 0;

    while (1) {
        char buf = 0;
        ssize_t bytes_read = read(fd, &buf, sizeof(char));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(4);
        }
        if (bytes_read == 0) {
            break;
        }

        if ((buf >= 'a' && buf <= 'z') || (buf >= 'A' && buf <= 'Z') || (buf >= '0' && buf <= '9')) {
            printf("sup %c\n", buf);
            shm_ptr[4 + n] = buf;
            n++;
        }
    }

    memcpy(shm_ptr, &n, sizeof(int));
    printf("sup n %d\n", *(int*)shm_ptr);

    close(fd);
}

int calculeaza_sum(char* shm_ptr) {
    int t1 = 0, t2 = 0;
    memcpy(&t1, shm_ptr, sizeof(int));
    memcpy(&t2, shm_ptr + sizeof(int), sizeof(int));
    int sum = t1 + t2;
    if (sum % 2 == 1) {
        return 1;
    }
    else {
        return 0;
    }
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

    parseaza_fisier(argv[1], shm_ptr);

    int ret_code = calculeaza_sum(shm_ptr);

    munmap("sup_to_w1", 4096);
    shm_unlink("sup_to_w1");
    close(shm_fd);

    return ret_code;
}
