#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

struct KeyVal {
    int key, val;
} instr[1024];
int size;

struct timespec clk;
long int get_time() {
    clock_gettime(CLOCK_MONOTONIC_RAW, &clk);
    // printf("%ld", clk.tv_nsec);
    return clk.tv_nsec;
}

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

void try_to_lock(int fd, struct flock* lock) {
    if (-1 == fcntl(fd, F_SETLKW, lock)) {
        if (errno == EINTR) {
            fprintf(stderr,"%d %ld Eroare, apelul fcntl a fost intrerupt de un semnal...", getpid(), get_time());
        }
        else {
            fprintf(stderr,"%d %ld Eroare la blocaj...", getpid(), get_time());
        }

        perror("\tMotivul erorii");
        exit(5);
    }
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
        printf("%d %ld Se cauta {%d,%d}\n", getpid(), get_time(), instr[i].key, instr[i].val);

        int found = 0;
        struct flock lock;
        lock.l_whence = SEEK_CUR;

        while (1) {
            rcode = read(fd, &pair.key, sizeof(pair.key));
            if (rcode == -1) {
                perror("Eroare la citirea din fisierul de date");
                exit(5); 
            }
            printf("%d %ld S-a citit {%d,?}\n", getpid(), get_time(), instr[i].key);

            if (rcode == 0) {
                // EOF
                break;
            }

            if (pair.key == instr[i].key) {
                printf("%d %ld S-a gasit {%d,%d}\n", getpid(), get_time(), instr[i].key, instr[i].val);

                lock.l_type = F_RDLCK;
                lock.l_start = 0;
                lock.l_len = sizeof(pair.val);
                printf("%d %ld Se inchide %d\n", getpid(), get_time(), lock.l_type);
                try_to_lock(fd, &lock);
                printf("%d %ld S-a inchis %d\n", getpid(), get_time(), lock.l_type);

                if (read(fd, &pair.val, sizeof(pair.val)) == -1) {
                    perror("Eroare la citirea din fisierul de date");
                    exit(5); 
                }
                printf("%d %ld S-a citit {%d,%d}\n", getpid(), get_time(), pair.key, pair.val);
                pair.val += instr[i].val;

                if (lseek(fd, -sizeof(pair.val), SEEK_CUR) == -1) {
                    perror("Eroare la pozitionarea cursorului");
                    exit(6);
                }

                lock.l_type = F_WRLCK;
                printf("%d %ld Se inchide %d\n", getpid(), get_time(), lock.l_type);
                try_to_lock(fd, &lock);
                printf("%d %ld S-a inchis %d\n", getpid(), get_time(), lock.l_type);

                if (write(fd, &pair.val, sizeof(pair.val)) == -1) {
                    perror("Eroare la scriere");
                    exit(7);
                }
                printf("%d %ld S-a scris {%d,%d}\n", getpid(), get_time(), pair.key, pair.val);
                usleep(500);

                found = 1;
            }
            else {
                if (lseek(fd, sizeof(pair.val), SEEK_CUR) == -1) {
                    perror("Eroare la pozitionarea cursorului");
                    exit(6);
                }
            }

            lock.l_type = F_UNLCK;
            lock.l_start = -sizeof(pair.val);
            printf("%d %ld Se inchide %d\n", getpid(), get_time(), lock.l_type);
            try_to_lock(fd, &lock);
            printf("%d %ld S-a inchis %d\n", getpid(), get_time(), lock.l_type);
        }

        if (!found) {
            printf("%d %ld Se adauga {%d,%d}\n", getpid(), get_time(), instr[i].key, instr[i].val);

            lock.l_type = F_WRLCK;
            lock.l_start = 0;
            lock.l_len = sizeof(instr[i]);
            printf("%d %ld Se inchide %d\n", getpid(), get_time(), lock.l_type);
            try_to_lock(fd, &lock);
            printf("%d %ld S-a inchis %d\n", getpid(), get_time(), lock.l_type);

            if (write(fd, &instr[i], sizeof(instr[i])) == -1) {
                perror("Eroare la scriere");
                exit(9);
            }
            printf("%d %ld S-a scris {%d,%d}\n", getpid(), get_time(), instr[i].key, instr[i].val);
            usleep(500);

            lock.l_type = F_UNLCK;
            lock.l_start = -sizeof(instr[i]);
            printf("%d %ld Se inchide %d\n", getpid(), get_time(), lock.l_type);
            try_to_lock(fd, &lock);
            printf("%d %ld S-a inchis %d\n", getpid(), get_time(), lock.l_type);
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
