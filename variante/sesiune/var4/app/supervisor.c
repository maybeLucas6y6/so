#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

struct char_freq {
    char c;
    int f;
};

void send_file(char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
        return;
    }
    while (1) {
        char buf = 0;
        ssize_t bytes_read = read(fd, &buf, sizeof(char));
        if (bytes_read == -1) {
            perror("Eroare la read");
            break;
        }
        if (bytes_read == 0) {
            break;
        }
        if (buf == '\n') {
            continue;
        }

        if (write(3, &buf, sizeof(char)) == -1) {
            perror("Eroare la write");
            break;
        }
    }
    close(fd);
    close(3);
}

void show_result(char* map) {
    struct char_freq voc_max, cons_min;
    while (1) {
        memcpy(&voc_max, map, sizeof(struct char_freq));
        memcpy(&cons_min, map + sizeof(struct char_freq), sizeof(struct char_freq));
        if (voc_max.f > 0 || cons_min.f > 0) {
            break;
        }
    }
    printf("voc %c max %d\ncons %c min %d\n",
            voc_max.c,
            voc_max.f,
            cons_min.c,
            cons_min.f);
}

int main(int argc, char* argv[]) {
    int w2_to_sup = shm_open("w2_to_sup", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (w2_to_sup == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }
    if (ftruncate(w2_to_sup, 2 * sizeof(struct char_freq)) == -1) {
        perror("Eroare la ftruncate");
        exit(4);
    }
    char* map = mmap(NULL, 2 * sizeof(struct char_freq), PROT_READ | PROT_WRITE, MAP_SHARED, w2_to_sup, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(3);
    }

    printf("sup incep sa scriu la w1 %ld\n", time(NULL));
    send_file(argv[1]);
    printf("sup terminat de scris la w1 %ld\n", time(NULL));

    //sleep(1);

    printf("sup incep sa citesc de la w2 %ld\n", time(NULL));
    show_result(map);
    printf("sup terminat de citit de la w2 %ld\n", time(NULL));

    munmap(map, 2 * (sizeof(int) + sizeof(char)));
    close(w2_to_sup);
    printf("sup exit\n");
    return 0;
}
