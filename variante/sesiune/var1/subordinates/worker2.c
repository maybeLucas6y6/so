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

    mini_impar = __INT_MAX__;
    maxi_par = __INT_MAX__ + 1;
    while (read(STDIN_FILENO, &res, sizeof(int)) == 4) {
        if (res % 2 == 0) {
            if (res > maxi_par) {
                maxi_par = res;
            }
        }
        else {
            if (res > mini_impar) {
                mini_impar = res;
            }
        }
    }

    min_max[0] = mini_impar;
    min_max[1] = maxi_par;
}

int main(int argc, char* argv[]) {
    printf("worker2\n");
    return 0;
    int fd_r_shmap = open("w2_to_sup", O_RDONLY | O_CREAT, 0600);
    if (fd_r_shmap == -1) {
        perror("Eroare la open");
        exit(4);
    }
    int* min_max = mmap(NULL, 8, PROT_WRITE, MAP_SHARED, fd_r_shmap, 0);
    if (min_max == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(5);
    }

    process(min_max);

    return 0;
}
