#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 2;
    }

    struct stat stats;
    if (stat(argv[1], &stats) == -1) {
        perror("Eroare la apelul functiei stat");
        return 3;
    }

    printf("%s:%ld\n", argv[1], stats.st_size);

    if ((stats.st_mode & S_IFMT) == S_IFDIR) {
        return 1;
    }
    else {
        return 0;
    }
}
