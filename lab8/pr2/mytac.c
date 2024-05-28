#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 1000

int main(int argc, char *argv[]) {
    int fd;

    fd = open(argv[1] , O_RDONLY);
    if (fd == -1) {
        perror(argv[1]);
        return 2;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error at fstat");
        return 4;
    }
    char* input_map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (input_map == MAP_FAILED) {
        perror("Error at mmap");
        return 4;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];

    int line_count = 0;
    char line[MAX_LINE_LENGTH];
    int i = sb.st_size, j;
    while (i >= 0) {
        if (!ignore_blank || strlen(line) > 1) {
            if (line_count < MAX_LINES) {
                strcpy(lines[line_count], line);
                line_count++;
            } else {
                fprintf(stderr, "Warning: Maximum line count reached, skipping remaining lines in file: %s\n", filename);
                break;
            }
        }
    }

    for (int i = line_count - 1; i >= 0; i--) {
        if (separator != NULL && strstr(lines[i], separator) == NULL) {
            continue;
        }
        fputs(lines[i], stdout);
    }

    for (int i = optind; i < argc; i++) {
        tac_file(argv[i], ignore_blank, separator);
    }

    return EXIT_SUCCESS;
}
