#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 2;
    }

    struct stat stats;
    if (stat(argv[1], &stats) == -1) {
        perror("stat");
        return 3;
    }

    printf("%s:%ld\n", argv[1], stats.st_size);

    // De aici
    printf("%s:", argv[1]);
    switch (stats.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  printf("directory\n");               break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
    }
    // Pana aici e optional

    if ((stats.st_mode & S_IFMT) == S_IFDIR) {
        return 1;
    }
    else {
        return 0;
    }
}
