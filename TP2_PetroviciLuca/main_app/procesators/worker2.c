#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void proceseaza_de_la_w1(int fifo, int pipe_fd) {
    int op = 0;
    while (1) {
        ssize_t bytes_read = 0;
        char buf = 0;
        read(fifo, &buf, sizeof(char));
        if (bytes_read == -1) {
            perror("Eroare la read");
            exit(4);
        }
        if (bytes_read == 0) {
            break;
        }

        if (isupper(buf)) {
            op = 0;
            buf = tolower(buf);
        }

        if (write(pipe_fd, &buf, sizeof(char)) == -1) {
            perror("Eroare la write in w2 1");
            exit(5);
        }
    }

    if (write(pipe_fd, &op, sizeof(int)) == -1) {
        perror("Eroare la write in w2 2");
        exit(6);
    }
}

int main(int argc, char* argv[]) {
    int pipe_fd[2] = { 0 };
    if (pipe(pipe_fd) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }
    
    pid_t pid_w1 = fork();
    if (pid_w1 == -1) {
        perror("Eroare la fork");
        exit(2);
    }
    else if (pid_w1 == 0) {
        close(pipe_fd[0]);
        if (dup2(pipe_fd[1], 10) == -1) {
            perror("Eroare la dup2");
            exit(3);
        }
        close(pipe_fd[1]);
        execl("main_app/procesators/worker1", "worker1", NULL);
        perror("Eroare la execl");
        exit(70);
    }

    close (pipe_fd[1]);

    if (mkfifo("w1_and_w2", 0600) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la mkfifo");
            exit(4);
        }
    }
    int w1_and_w2 = open("w1_and_w2", O_WRONLY);
    if (w1_and_w2 == -1) {
        perror("Eroare la open");
        exit(5);
    }

    proceseaza_de_la_w1(w1_and_w2, pipe_fd[0]);

    close(pipe_fd[0]);
    close(w1_and_w2);

    return 0;
}
