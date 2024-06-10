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
#include <time.h>

void calc_freq(int sup_to_w1, int w1_to_w2) {
    printf("w1 incep sa citesc de la sup %ld\n", time(NULL));
    int freq[26] = { 0 };
    while (1) {
        char buf = 0;
        ssize_t bytes_read = read(sup_to_w1, &buf, sizeof(char));
        if (bytes_read == -1) {
            perror("Eroare la read");
            break;
        }
        if (bytes_read == 0) {
            break;
        }

        freq[tolower(buf) - 'a']++;
    }
    printf("w1 terminat de citit de la sup %ld\n", time(NULL));

    printf("w1 incep sa scriu la sup %ld\n", time(NULL));
    struct char_freq {
        char c;
        int f;
    };
    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            struct char_freq buf;
            buf.c = 'a' + i;
            buf.f = freq[i];
            if (write(w1_to_w2, &buf, sizeof(struct char_freq)) == -1) {
                perror("Eroare la write");
                break;
            }
        }
    }
    printf("w1 terminat de scris la sup %ld\n", time(NULL));
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Numar invalid de argumente");
        exit(1);
    }

    int sup_to_w1[2] = { 0 };
    if (pipe(sup_to_w1) == -1) {
        perror("Eroare la pipe");
        exit(2);
    }

    pid_t sup = fork();
    if (sup == -1) {
        perror("Eroare la fork");
        exit(3);
    }
    else if (sup == 0) {
        close(sup_to_w1[0]);
        if (dup2(sup_to_w1[1], 3) == -1) {
            perror("Eroare la dup2");
            exit(7);
        }
        close(sup_to_w1[1]);
        execl("app/supervisor", "supervisor", argv[1], NULL);
        exit(10);
    }

    close(sup_to_w1[1]);

    if (mkfifo("w1_to_w2", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(4);
        }
    }
    int w1_to_w2 = open("w1_to_w2", O_WRONLY);
    if (w1_to_w2 == -1) {
        perror("Eroare la open");
        exit(5);
    }

    calc_freq(sup_to_w1[0], w1_to_w2);

    close(sup_to_w1[0]);
    close(w1_to_w2);
    printf("w1 exit\n");
    return 0;
}
