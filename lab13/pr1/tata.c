#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>

int n, ok = 0, ticks = 0, quit = 0;

void handle_alarm(int sig_code) {
    ticks++;
    if (!quit && ticks == 5) {
        printf("Ies\n");
        exit(68);
    }
    else if (!ok) {
        printf("Scrie\n");
    }
    ok = 0;
    alarm(n);
}

void handle_usr2(int sig_code) {
    printf("Am primit SIGUSR2\n");
}

void handle_chld(int sig_code) {
    printf("Am primit SIGCHLD\n");
}

int main(int argc, char* argv[]) {
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    n = atoi(argv[2]);

    if (fd == -1) {
        perror("Eroare la open");
        return 1;
    }

    signal(SIGALRM, handle_alarm);
    alarm(n);

    char buf = 0;
    while (read(STDIN_FILENO, &buf, sizeof(char)) > 0) {
        ok = 1;
        quit = 1;
        write(fd, &buf, sizeof(char));
    }
    close(fd);

    pid_t pid_fiu = fork();
    if (pid_fiu == -1) {
        perror("Eroare la fork");
        return 2;
    }
    else if (pid_fiu == 0) {
        printf("Fiu: start\n");

        signal(SIGUSR2, handle_usr2);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR2);
        sigsuspend(&mask);

        printf("Fiu: Acum afisez fisierul\n");
        if ((fd = open(argv[1], O_RDONLY)) == -1) {
            perror("Eroare la deschiderea fisierului!");
            return 4;
        }

        while (read(fd, &buf, sizeof(char)) > 0) {
            printf("%c", buf);
        }

        close(fd);

        printf("Fiu: Sfarsit executie\n");
        return 0;
    }
    else {
        sleep(1);
        printf("Tata: transmit fiului SIGUSR2\n");
        if (kill(pid_fiu, SIGUSR2) == -1) {
            perror("Eroare la kill");
            return 3;
        }

        printf("Tata: astept fiul sa dea SIGCHLD\n");

        signal(SIGCHLD, handle_chld);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGCHLD);
        sigsuspend(&mask);

        printf("Tata: gata\n");
    }

    return 0;
}

