#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 2;
    }

    if (access(argv[1], R_OK) == -1) {
        return 3;
    }

    int fd;
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Eroare la apelul open\n");
        return 3;
    }

    int mici = 0, mari = 0;
    char ch;
    int read_code;
    while (1) {
        read_code = read(fd, &ch, 1);
        if (read_code == -1) {
            perror("Eroare la apelul read\n");
            return 4;
        }
        if (read_code == 0) {
            break; // EOF
        }

        if (ch >= 'a' && ch <= 'z') {
            mici++;
        }
        else if (ch >= 'A' && ch <= 'Z') {
            mari++;
        }
    }

    printf("%s:%d:%d\n", argv[1], mari, mici);
    return 0;
}
