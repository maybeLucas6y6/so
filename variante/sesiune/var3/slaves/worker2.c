#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void numara(int w1_to_w2, int* w2_to_sup) {
    int voc = 0, cons = 0;
    while (1) {
        char c = 0;
        if (read(w1_to_w2, &c, sizeof(char)) <= 0) {
            break;
        }
        printf("%c", c);

        if (strchr("aeiouAEIOU", c) != NULL) {
            voc++;
        }
        else {
            cons++;
        }
    }
    w2_to_sup[0] = voc;
    w2_to_sup[1] = cons;
}

int main(int argc, char* argv[]) {
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

    if (mkfifo("send_data", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(4);
        }
    }
    int w1_to_w2 = open("send_data", O_RDONLY);
    if (w1_to_w2 == -1) {
        perror("Eroare la open");
        exit(5);
    }

    numara(w1_to_w2, map);

    close (w1_to_w2);
    munmap(map, 2 * sizeof(int));
    close(w2_to_sup);
    return 1;
}
