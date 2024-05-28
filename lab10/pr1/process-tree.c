#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid_fii[12] = {};
    int current = -1;
    int status;

    for (int i = 0; current == -1 && i < 2; i++) {
        if ((pid_fii[i] = fork()) == -1) {
            perror("Eroare la fork");
            return 1;
        }
        
        if (pid_fii[i] == 0) {
            current = i;
        }
    }

    // 0 1
    // 2 3 4 5 6
    // 7 8 9 0 1
    if (current >= 0 && pid_fii[current] == 0) {
        // fiu
        for (int i = 0; current < 2 && i < 5; i++) {
            int j = current * 5 + 2 + i;
            if ((pid_fii[j] = fork()) == -1) {
                perror("Eroare la fork");
                return 1;
            }

            if (pid_fii[j] == 0) {
                current = j;
            }
        }

        if (current >= 2 && pid_fii[current] == 0) {
            // frunza
            printf("(3, %d) PID:%d PPID:%d nu are fii\n", current - 1, getpid(), getppid());
            return 0;
        }

        for (int i = 0; i < 5; i++) {
            int j = current * 5 + 2 + i;
            pid_fii[j] = wait(&status);

            if (pid_fii[j] == -1) {
                perror("Eroare la wait");
                return 2;
            }
        }
        printf("(2, %d) PID:%d PPID:%d", current, getpid(), getppid());
        for (int i = 0; i < 5; i++) {
            int j = current * 5 + 2 + i;
            printf(", PID:%d:%d", pid_fii[j], status >> 8);
        }
        printf("\n");
    }
    else {
        // parinte - root
        sleep(1);
        for (int i = 0; i < 2; i++) {
            pid_fii[i] = wait(&status);

            if (pid_fii[i] == -1) {
                perror("Eroare la wait");
                return 2;
            }
        }
        printf("(1, 1) PID:%d PPID:%d", getpid(), getppid());
        for (int i = 0; i < 2; i++) {
            printf(", PID:%d:%d", pid_fii[i], status >> 8);
        }
        printf("\n");
    }

    return 0;
}
