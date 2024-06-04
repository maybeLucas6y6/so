#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    pid_t pids[4] = { 0 };
    int pipes[3][2] = { 0 };

    if (pipe(pipes[0]) == -1) {
        fprintf(stderr, "Eroare: nu pot crea primul canal anonim, errno=%d\n",errno);  perror("Cauza erorii");  exit(1);
    }
    if (pipe(pipes[2]) == -1) {
        fprintf(stderr, "Eroare: nu pot crea primul canal anonim, errno=%d\n",errno);  perror("Cauza erorii");  exit(1);
    }
    if (pipe(pipes[1]) == -1) {
        fprintf(stderr, "Eroare: nu pot crea al doilea canal anonim, errno=%d\n",errno);  perror("Cauza erorii");  exit(1);
    }

    if ((pids[0] = fork()) == -1) {
        perror("Eroare la fork");  exit(1);
    }

    if (pids[0] == 0) {
        if (dup2(pipes[0][1], 1) == -1) {
            perror("Eroare: redirectarea iesirii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
        }

        execlp("w", "w", "-h", NULL);
        perror("Eroare la execlp() in al doilea proces fiu. Cauza erorii");  exit(5);
    }
    else {
        if ((pids[1] = fork()) == -1) {
            perror("Eroare la fork");  exit(1);
        }

        if (pids[1] == 0) {
            close(pipes[0][1]);

            if (dup2(pipes[0][0], 0) == -1) {
                perror("Eroare: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
            }

            if (dup2(pipes[1][1], 1) == -1) {
                perror("Eroare: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
            }

            execlp("tr", "tr", "-s", "\" \"", NULL);
            perror("Eroare la execlp() in al doilea proces fiu. Cauza erorii");  exit(5);
        }
        else {
            if ((pids[2] = fork()) == -1) {
                perror("Eroare la fork");  exit(1);
            }

            if (pids[2] == 0) {
                close(pipes[0][1]);
                close(pipes[1][1]);

                if (dup2(pipes[1][0], 0) == -1) {
                    perror("Eroare: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
                }

                if (dup2(pipes[2][1], 1) == -1) {
                    perror("Eroare: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
                }

                execlp("cut", "cut", "-d ", "-f1,8", NULL);
                perror("Eroare la execlp() in al doilea proces fiu. Cauza erorii");  exit(5);
            }
            else {
                if ((pids[3] = fork()) == -1) {
                    perror("Eroare la fork");  exit(1);
                }

                if (pids[3] == 0) {
                    close(pipes[0][1]);
                    close(pipes[1][1]);
                    close(pipes[2][1]);

                    if (dup2(pipes[2][0], 0) == -1) {
                        perror("Er3are: redirectarea intrarii standard in al doilea proces fiu a esuat. Cauza erorii");  exit(4);
                    }

                    execlp("sort", "sort", "-t", " ", "-k", "1", NULL);
                    perror("Eroare la execlp() in al doilea proces fiu. Cauza erorii");  exit(5);
                }
            }
        }
    }

    return 0;
}
