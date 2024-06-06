#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


void encripteaza(int w1_to_w2) {
    while (1) {
        char c = 0;
        if (read(STDIN_FILENO, &c, sizeof(char)) <= 0) {
            break;
        }

        char k = 0;
        if ((c >= 'a' && c <= 'z')) {
            k = (c - 'a' + 14) % 26 + 'a';
        }
        else {
            k = (c - 'A' + 14) % 26 + 'A';
        }
        if (write(w1_to_w2, &k, sizeof(char)) != 1) {
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (mkfifo("send_data", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(4);
        }
    }
    int w1_to_w2 = open("send_data", O_WRONLY);
    if (w1_to_w2 == -1) {
        perror("Eroare la open");
        exit(5);
    }

    encripteaza(w1_to_w2);

    close(w1_to_w2);
    return 0;
}
