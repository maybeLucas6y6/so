#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAP_NAME_SIZE 32
#define MAP_MAX_SIZE 4096

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    int* shmaps_fds = malloc(N * sizeof(int));
    char** shmaps_names = malloc(N * sizeof(char*));
    for (int i = 0; i < N; i++) {
        shmaps_names[i] = malloc(MAP_NAME_SIZE);
    }
    char** shmaps_ptrs = malloc(N * sizeof(char*));

    for (int i = 0; i < N; i++) {
        snprintf(shmaps_names[i], MAP_NAME_SIZE, "%d", i + 400);
        shmaps_fds[i] = shm_open(shmaps_names[i], O_CREAT | O_RDWR | O_TRUNC, 0666);
        if (shmaps_fds[i] == -1) {
            perror("Eroare la crearea unei mape!");
            return -1;
        }
        if (ftruncate(shmaps_fds[i], MAP_MAX_SIZE)) {
            perror("Eroare la truncate!");
            return -1;
        }
        shmaps_ptrs[i] = mmap(NULL, MAP_MAX_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shmaps_fds[i], 0);
    }

    int fd = open(argv[2], O_RDONLY);
    char* fd_map = mmap(NULL, MAP_MAX_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);

    int newlines_cnt = 0; 
    for (unsigned long i = 0; i < strlen(fd_map); i++) {
        if (fd_map[i] == '\n') {
            newlines_cnt++;
        }
    }
    printf("Sup: Found %d newlines\n", newlines_cnt);
    int avg_chunk_size = newlines_cnt / N;

    int last_newline = -1;
    for (unsigned long i = 0, n_cnt = 0, worker = 0; i < strlen(fd_map); i++) {
        if (fd_map[i] == '\n') {
            n_cnt++;
            if (n_cnt == avg_chunk_size) {
                int chunk_offset = last_newline + 1;
                int chunk_size = i - last_newline;
                printf("worker %lu, chunk_offset %d, chunk_size %d\n", worker, chunk_offset, chunk_size);
                memcpy(shmaps_ptrs[worker], &chunk_offset, sizeof(chunk_offset));
                memcpy(shmaps_ptrs[worker] + sizeof(chunk_offset), &chunk_size, sizeof(chunk_size));
                memcpy(shmaps_ptrs[worker] + sizeof(chunk_offset) + sizeof(chunk_size), argv[2], strlen(argv[2]) + 1);
                n_cnt = 0;
                worker++;
                last_newline = i;
            }
        }
    }

    pid_t* workers_pids = malloc(N * sizeof(pid_t));
    for (int i = 0; i < N; i++) {
        workers_pids[i] = fork(); 
        if (workers_pids[i] == -1) {
            perror("Eroare la fork");
            return 700;
        }
        if (workers_pids[i] == 0) {
            execl("worker_sort", "worker_sort", shmaps_names[i], NULL);
            perror("Eroare la exec");
            exit(10);
        }
    }

    int successful_workers = 0;
    for (int i = 0; i < N; i++) {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            switch (WEXITSTATUS(status)) {
                case 10:  printf("Apelul exec a esuat\n");  break;
                case  0:  printf("Success!\n");  successful_workers++; break;
                default:  printf("Worker-ul a fost executat, dar a esuat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status));
            }
        }
        else {
            printf("Worker-ul a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status) );
        }
    }

    if (successful_workers == N) {
        for (int i = 0; i < N; i++) {
            printf("worker %d result: %s\n", i, shmaps_ptrs[i] + (2 * sizeof(int)));
        }
    }

    munmap(fd_map, MAP_MAX_SIZE);
    close(fd);
    for (int i = 0; i < N; i++) {
        munmap(shmaps_ptrs[i], MAP_MAX_SIZE);
        close(shmaps_fds[i]);
    }

    free(workers_pids);
    free(shmaps_ptrs);
    for (int i = 0; i < N; i++) {
        free(shmaps_names[i]);
    }
    free(shmaps_names);
    free(shmaps_fds);
    printf("zzz\n");
}
