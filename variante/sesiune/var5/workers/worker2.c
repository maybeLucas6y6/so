#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void power2(int* map) {
    printf("w2 map0 %d\n", map[0]);
    int n = 0;
    while (1) {
        if (map[0] != 0) {
            n = map[0];
            printf("w2 continui %d %d\n", n, map[0]);
            break;
        }
        else {
            printf("w2 map0 %d\n", map[0]);
        }
        sleep(1);
    }
    printf("w2 n %d\n", n);
    for (int i = 1; i <= n; i++) {
        printf("w2 map %d %d\n", i, map[i]);
        map[i] = (map[i] * map[i]);
    }
}

int main(int argc, char* argv[]) {
    printf("w2 start\n");
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

    power2(map);

    if (munmap(map, 4096) == -1) {
        perror("Eroare la munmap");
        exit(4);
    }
    shm_unlink("w1_and_w2");
    close(fd_map);
    printf("w2 exit\n");
    return 0;
}
