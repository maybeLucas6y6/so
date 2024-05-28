#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAP_MAX_SIZE 4096

struct stringview {
    char* start;
    int size;
};

int stringview_cmp(const struct stringview a, const struct stringview b) {
    int i = 0;
    while (i < a.size && i < b.size) {
        if (a.start[i] < b.start[i]) {
            return -1;
        }
        if (a.start[i] > b.start[i]) {
            return 1;
        }
        i++;
    }

    if (a.size < b.size) {
        return -1;
    }
    if (a.size > b.size) {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "400") == 0) {
        return 0;
    }

    int map_fd = shm_open(argv[1], O_RDWR, 0666);
    char* map_ptr = mmap(NULL, MAP_MAX_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, map_fd, 0);
    if (map_ptr == MAP_FAILED) {
        perror("Eroare la deschiderea mapei");
        return 1;
    }
    //printf("map: %s", map_ptr);

    int chunk_offset = 0, chunk_size = 0;
    char* filename = map_ptr + (sizeof(chunk_offset) + sizeof(chunk_size));
    memcpy(&chunk_offset, map_ptr, sizeof(chunk_offset));
    memcpy(&chunk_size, map_ptr + sizeof(chunk_offset), sizeof(chunk_size));
    /*printf("chunk offset: %d, chunk size: %d, filename: %s\n", 
           chunk_offset, 
           chunk_size,
           filename);
           */
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la deschidere");
        return 2;
    }

    if (lseek(fd, chunk_offset, SEEK_SET) == -1) {
        perror("Eroare la lseek");
        return 3;
    }

    if (read(fd, filename, chunk_size) == -1) {
        perror("Eroare la read");
        return 4;
    }
    close(fd);

    int newlines = 0;
    for (int i = 0; i < chunk_size; i++) {
        if (filename[i] == '\n') {
            newlines++;
        }
    }
    struct stringview* lines = malloc(newlines * sizeof(struct stringview));
    int prev_line_end = 0;
    for (int i = 0, line = 0; i < chunk_size; i++) {
        if (filename[i] == '\n') {
            lines[line].start = filename + prev_line_end;
            lines[line].size = i - prev_line_end + 1;
            prev_line_end = i + 1;
            line++;
        }
    }

    printf("[%s] %d\n", argv[1], newlines);

    for (int i = 0; i < newlines; i++) {
        /*
        printf("line %d %d|%d: %.*s",
                i,
                (int)(lines[i].start - filename),
                lines[i].size,
                lines[i].size,
                lines[i].start);
                */
        printf("|%.*s ", lines[i].size - 1, lines[i].start);
    }

    for (int i = 0; i < newlines - 1; i++) {
        for (int j = i + 1; j < newlines; j++) {
            if (stringview_cmp(lines[i], lines[j]) < 0) {
                char* a = malloc(lines[i].size);
                char* b = malloc(lines[j].size);
                int offset = lines[j].size - lines[i].size;
                memcpy(a, lines[i].start, lines[i].size);
                memcpy(b, lines[j].start, lines[j].size);
                printf("\n---start---\n");

                for (int k = i + 1; k < j; k++) {
                    lines[k].start += offset;
                }
                    memcpy(lines[i].start + lines[j].size,
                            lines[i].start + lines[i].size,
                            lines[j].start - (lines[i].start + lines[i].size));
                    lines[j].start += offset;
                for (int k = 0; k < newlines; k++) {
                    /*
                    printf("line %d %d|%d: %.*s",
                            k,
                            (int)(lines[k].start - filename),
                            lines[k].size,
                            lines[k].size,
                            lines[k].start);
                            */
                    printf("|%.*s ", lines[k].size - 1, lines[k].start);
                }
                printf("\n---middle---\n");
                memcpy(lines[i].start, b, strlen(b));
                memcpy(lines[j].start, a, strlen(a));
                int tmp = lines[i].size;
                lines[i].size = lines[j].size;
                lines[j].size = tmp;
                for (int k = 0; k < newlines; k++) {
                    /*
                    printf("line %d %d|%d: %.*s",
                            k,
                            (int)(lines[k].start - filename),
                            lines[k].size,
                            lines[k].size,
                            lines[k].start);
                            */
                    printf("|%.*s ", lines[k].size - 1, lines[k].start);
                }
                printf("\n---end---\n");

                /*
                char* tmp = lines[i].start;
                lines[i].start = lines[j].start;
                lines[j].start = tmp;


                01234567890123456789012345678901345678901234567890
                0       8   23    8      5      3 5 7    2   6
                aaaaaaaabbbbcdddddeeeeeeefffffffgghhiiiiijjjjkkkkk
                        ----++++++-------
                        8,4       18,7         off: 3

                aaaaaaaabbbb***cdddddeeeefffffffgghhiiiiijjjjkkkkk

                */

                free(b);
                free(a);
            }
        }
    }

    free(lines);
    return 0;
}
