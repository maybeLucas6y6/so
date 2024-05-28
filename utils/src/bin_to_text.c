#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct KeyVal {
    int key, val;
};

int main(int argc, char* argv[]) {
    int fd, rcode;

    if (argc == 1) {
        printf("Numar gresit de parametri\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Eroare la deschiderea fisierului de date");
        return 2;
    }

    struct KeyVal i;

    while ((rcode = read(fd, &i, sizeof(i))) != -1) {
        if (rcode == 0) {
            break;
        }

        printf("%d, %d\n", i.key, i.val);
    }

    close(fd);
    return 0;
}
