#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void send_file(char* filename, int sup_to_w1) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la open");
    }

    while (1) {
        char buf[32] = { 0 };
        int len = 0;
        ssize_t bytes_read = 0;
        while (1) {
            bytes_read = read(fd, buf + len, sizeof(char));
            if (bytes_read == -1) {
                perror("Eroare la read sup send_file");
                exit(10);
            }
            if (bytes_read == 0) {
                break;
            }
            if (buf[len] < '0' || buf[len] > '9') {
                printf("sup endnum\n");
                buf[len] = 0;
                break;
            }
            len++;
        }
        if (bytes_read == 0) {
            break;
        }

        int num = atoi(buf);
        printf("sup num %d\n", num);

        if (write(sup_to_w1, &num, sizeof(int)) != sizeof(int)) {
            perror("Eroare la write");
            exit(11);
        }
    }

    if (close(fd) == -1) {
        perror("Eroare la close");
    }
}

void show_result(int w1_to_sup) {
    int sum = 0;
    while (1) {
        int num = 0;
        ssize_t bytes_read = read(w1_to_sup, &num, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read sup show_result");
            exit(12);
        }
        if (bytes_read == 0) {
            break;
        }
        sum += num;
    }
    printf("sum %d\n", sum);
    // conv to text, afisare sum cifre
}

int main(int argc, char* argv[]) {
    printf("sup start\n");
    if (argc != 2) {
        printf("Use ./supervisor [path]\n");
        exit(1);
    }

    int sup_to_w1[2] = { 0 };
    if (pipe(sup_to_w1) == -1) {
        perror("Eroare la pipe");
        exit(2);
    }

    pid_t w1_pid = fork();
    if (w1_pid == -1) {
        perror("Eroare la fork");
        exit(3);
    }
    else if (w1_pid == 0) {
        if (close(sup_to_w1[1]) == -1) {
            perror("Eroare la close");
            exit(8);
        }
        if (dup2(sup_to_w1[0], 10) == -1) {
            perror("Eroare la dup2");
            exit(4);
        }
        if (close(sup_to_w1[0]) == -1) {
            perror("Eroare la close");
            exit(8);
        }
        execl("workers/worker1", "worker1", NULL);
        printf("Eroare la execl");
        exit(10);
    }

    if (close(sup_to_w1[0]) == -1) {
        perror("Eroare la close");
        exit(8);
    }

    if (mkfifo("w1_to_sup", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(5);
        }
    }
    int w1_to_sup = open("w1_to_sup", O_RDONLY);
    if (w1_to_sup == -1) {
        perror("Eroare la open");
        exit(6);
    }

    send_file(argv[1], sup_to_w1[1]);

    show_result(w1_to_sup);

    if (close(w1_to_sup) == -1) {
        perror("Eroare la close");
        exit(7);
    }
    if (close(sup_to_w1[1]) == -1) {
        perror("Eroare la close");
        exit(8);
    }
    return 0;
}
