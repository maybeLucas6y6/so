#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct KeyVal {
    int key, val;
} instr[1024];
int size;

void read_instructions(const char* file_name) {
    int fd, rcode;

    if ((fd = open(file_name, O_RDONLY)) == -1) {
        perror("Eroare la deschiderea fisierului de date");
        exit(1);
    }

    while (1) {
        rcode = read(fd, &instr[size], sizeof(instr[size]));
        if (rcode == -1) {
            perror("Eroare la citirea din fisierul de date");
            exit(2);
        }

        if (rcode == 0) {
            // EOF
            break;
        }

        size++;
    }

    close(fd);
}

void execute_instructions(const char* file_name) {
    int fd, rcode;

    if ((fd = open(file_name, O_RDWR)) == -1) {
        perror("Eroare la deschiderea fisierului de date");
        exit(3); 
    }

    struct KeyVal pair;

    for(int i = 0; i < size; i++) {
        if (lseek(fd, 0, SEEK_SET) == -1) {
            perror("Eroare la pozitionarea cursorului");
            exit(4); 
        }
        int found = 0;
        printf("[%d] Se cauta {%d,%d}\n", getpid(), instr[i].key, instr[i].val);

        while (1) {
            rcode = read(fd, &pair, sizeof(pair));
            if (rcode == -1) {
                perror("Eroare la citirea din fisierul de date");
                exit(5); 
            }

            if (rcode == 0) {
                // EOF
                break;
            }

            if (pair.key == instr[i].key) {
                printf("[%d] S-a gasit {%d,%d}\n", getpid(), pair.key, pair.val);
                if (lseek(fd, -sizeof(pair.val), SEEK_CUR) == -1) {
                    perror("Eroare la pozitionarea cursorului");
                    exit(6);
                }

                pair.val += instr[i].val;

                if (write(fd, &pair.val, sizeof(pair.val)) == -1) {
                    perror("Eroare la scriere");
                    exit(7);
                }
                usleep(500);

                found = 1;
            }
        }

        if (!found) {
            printf("[%d] Se adauga {%d,%d}\n", getpid(), instr[i].key, instr[i].val);

            if (write(fd, &instr[i], sizeof(instr[i])) == -1) {
                perror("Eroare la scriere");
                exit(9);
            }
            usleep(500);
        }
    }

    close(fd);
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf("Numar de argumente gresit");
        return 1;
    }

    read_instructions(argv[2]);
    execute_instructions(argv[1]);

    return 0;
}
