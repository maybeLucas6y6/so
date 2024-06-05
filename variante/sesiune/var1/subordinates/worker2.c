#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void process(int* min_max) {
    int res = 0, mini_impar = 0, maxi_par = 0;
    if (read(STDIN_FILENO, &res, sizeof(int)) == -1) {
        perror("Eroare la read");
        exit(1);
    }

    mini_impar = 99999;
    maxi_par = -99999;
    while (read(STDIN_FILENO, &res, sizeof(int)) == 4) {
        if (abs(res) % 2 == 0) {
            if (res > maxi_par) {
                maxi_par = res;
            }
        }
        else {
            if (abs(res) > mini_impar) {
                mini_impar = res;
            }
        }
    }

    min_max[0] = mini_impar;
    min_max[1] = maxi_par;
}

int main(int argc, char* argv[]) {
    int fd_r_shmap = shm_open("w2_to_sup", O_RDWR | O_CREAT, 0600);
    if (fd_r_shmap == -1) {
        perror("Eroare la open");
        exit(4);
    }
    int* min_max = mmap(NULL, 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd_r_shmap, 0);
    if (min_max == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(5);
    }

    process(min_max);

    close(STDIN_FILENO);
    close(fd_r_shmap);
    munmap(min_max, 8);

    return 0;
}
