#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

void handle_usr2_sort(int signal) {
    printf("sort received SIGUSR2\n");
}

void handle_usr2_sum(int signal) {
    printf("sum received SIGUSR2\n");
}

void handle_usr1_sort(int signal) {
    printf("sort received SIGUSR1\n");
}

void handle_usr1_sum(int signal) {
    printf("sum received SIGUSR1\n");
}

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);
    int* map = mmap(NULL, n * sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    srandom(time(NULL));
    for (int i = 0; i < n; i++) {
        map[i] = random() % 100;
        printf("%d ", map[i]);
    }
    printf("\n");

    pid_t fiu_suma = 0, fiu_sortare = 0;

    fiu_sortare = fork();
    if (fiu_sortare == -1) {
        perror("Eroare la fork");
        exit(1);
    }
    else if (fiu_sortare == 0) {
        printf("sort start\n");

        sigset_t mask;

        sigfillset(&mask);
        sigdelset(&mask, SIGUSR2);
        signal(SIGUSR2, handle_usr2_sort);
        sigsuspend(&mask);

        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        signal(SIGUSR1, handle_usr1_sort);
        sigsuspend(&mask);

        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                if (map[i] < map[j]) {
                    int aux = map[i];
                    map[i] = map[j];
                    map[j] = aux;
                }
            }
        }
        for (int i = 0; i < n; i++) {
            printf("%d ", map[i]);
        }
        printf("\n");

        kill(fiu_suma, SIGUSR1);

        printf("sort exit\n");
        exit(0);
    }

    fiu_suma = fork();
    if (fiu_suma == -1) {
        perror("Eroare la fork");
        exit(2);
    }
    else if(fiu_suma == 0) {
        printf("suma start\n");

        sigset_t mask;

        sigfillset(&mask);
        sigdelset(&mask, SIGUSR2);
        signal(SIGUSR2, handle_usr2_sum);
        sigsuspend(&mask);

        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += map[i];
        }
        printf("sum: %d\n", sum);
        kill(fiu_sortare, SIGUSR1);

        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        signal(SIGUSR1, handle_usr1_sum);
        sigsuspend(&mask);

        printf("suma exit\n");
        exit(0);
    }

    sleep(1);
    kill(fiu_sortare, SIGUSR2);
    kill(fiu_suma, SIGUSR2);

    munmap(map, n * sizeof(int));
    return 0;
}
