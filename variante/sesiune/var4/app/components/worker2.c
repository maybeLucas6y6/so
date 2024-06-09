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

struct char_freq {
    char c;
    int f;
};

void numara_consoane_vocale(int w1_to_w2, char* w2_to_sup) {
    struct char_freq voc_max = { 'z', 0 }, cons_min = { 'z', 9999 };
    while (1) {
        struct char_freq buf;
        ssize_t bytes_read = read(w1_to_w2, &buf, sizeof(struct char_freq));
        if (bytes_read == -1) {
            perror("Eroare la read");
            break;
        }
        if (bytes_read == 0) {
            break;
        }

        buf.c = tolower(buf.c);
        if (strchr("aeiouAEIOU", buf.c) != NULL) {
            if (buf.f > voc_max.f || (buf.f == voc_max.f && buf.c < voc_max.c)) {
                memcpy(&voc_max, &buf, sizeof(struct char_freq));
            }
        }
        else {
            if (buf.f < cons_min.f || (buf.f == cons_min.f && buf.c < cons_min.c)) {
                memcpy(&cons_min, &buf, sizeof(struct char_freq));
            }
        }
    }

    memcpy(w2_to_sup, &voc_max, sizeof(struct char_freq));
    memcpy(w2_to_sup + sizeof(struct char_freq), &cons_min, sizeof(struct char_freq));
}

int main(int argc, char* argv[]) {
    if (mkfifo("w1_to_w2", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(1);
        }
    }
    int w1_to_w2 = open("w1_to_w2", O_RDONLY);
    if (w1_to_w2 == -1) {
        perror("Eroare la open");
        exit(2);
    }

    int w2_to_sup = shm_open("w2_to_sup", O_RDWR | O_CREAT, 0600);
    if (w2_to_sup == -1) {
        perror("Eroare la shm_open");
        exit(3);
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

    numara_consoane_vocale(w1_to_w2, map);

    munmap(map, 2 * sizeof(struct char_freq));
    close(w2_to_sup);
    close(w1_to_w2);
    return 0;
}
