#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void citire_operatii(int fd, int pipe) {
    while (1) {
        int t1 = 0, t2 = 0;
        char op = 0;
        int bytes_read = 0;
        bytes_read = read(fd, &t1, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(100);
        }
        if (bytes_read == 0 || bytes_read != sizeof(int)) {
            break;
        }

        bytes_read = read(fd, &op, sizeof(char));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(100);
        }
        if (bytes_read == 0 || bytes_read != sizeof(char)) {
            break;
        }

        bytes_read = read(fd, &t2, sizeof(int));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(100);
        }
        if (bytes_read == 0 || bytes_read != sizeof(int)) {
            break;
        }

        int res = 0;
        switch (op) {
            case '+': res = t1 + t2; break;
            case '-': res = t1 - t2; break;
            case '*': res = t1 * t2; break;
            case '/': res = t1 / t2; break;
        }

        if (write(pipe, &res, sizeof(int)) == -1) {
            perror("Eroare la write");
            exit(101);
        }
    }
}

int main(int argc, char* argv[]) {
    printf("worker1\n");
    int w1_to_w2[2] = { 0 };
    if (pipe(w1_to_w2) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    pid_t pid_fiu = fork();

    if (pid_fiu == -1) {
        perror("Eroare la fork");
        exit(2);
    }
    else if (pid_fiu == 0) {
        close(w1_to_w2[1]);
        if (dup2(w1_to_w2[0], STDIN_FILENO) == -1) {
            perror("Eroare la dup2");
            exit(3);
        }
        printf("incerc sa fac worker2\n");
        execl("worker2", "worker2", NULL);

        exit(69);
    }

    close(w1_to_w2[0]);

    if (mkfifo("sup_to_w1", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Aoleu");
            exit(4);
        }
    }

    int fd_r_fifo = open("sup_to_w1", O_WRONLY);
    if (fd_r_fifo == -1) {
        perror("Eroare la open");
        exit(5);
    }

    citire_operatii(fd_r_fifo, w1_to_w2[1]);
    close(w1_to_w2[1]);

    close(fd_r_fifo);
    return 0;
}
