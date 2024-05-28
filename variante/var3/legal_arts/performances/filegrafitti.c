#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 2;
    }

    int fd;
    if ((fd = open(argv[1], O_RDWR)) == -1) {
        printf("Eroare la deschiderea fisierului\n");
        return 3;
    }

    struct stat stats;
    if (stat(argv[1], &stats) == -1) {
        printf("Eroare la apelul functiei stat\n");
        return 4;
    }
    unsigned long N = strlen(argv[2]);
    if (N > stats.st_size) {
        printf("CANVAS TOO SMALL\n");
        return 5;
    }

    if (lseek(fd, -N - 1, SEEK_END) == -1) {
        printf("Eroare la apelul functiei lseek\n");
        return 6;
    }

    if (write(fd, argv[2], N) == -1) {
        perror("Eroare la scriere in fisier\n");
        return 7;
    }

    printf("FILE VANDALIZED: %s\n", strrchr(argv[1], '/') + 1);
    return 0;
}
