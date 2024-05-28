#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int fd;

    if (argc == 1) {
        printf("Numar gresit de parametri\n");
        return 1;
    }

    if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror("Eroare la deschiderea fisierului de date");
        return 2;
    }

    for (int i = 2; i < argc; i++) {
        int nr = 0;
        if (1 != sscanf(argv[i], "%d", &nr)) {
            perror("Eroare la conversie");
            return 4;
        }

        if (write(fd, &nr, sizeof(int)) == -1) {
            perror("Eroare la scriere");
            return 3;
        }
    }

    close(fd);
}
