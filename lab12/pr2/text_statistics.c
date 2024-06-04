#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    int pipes[2][2] = { 0 };

	if (pipe(pipes[0]) == 1) {
		perror("Eroare la crearea pipe-ului intre 0 si 1!");
		exit(2);
	}
	if (pipe(pipes[1]) == 1) {
		perror("Eroare la crearea pipe-ului intre 2 si 0!");
		exit(3);
	}

	if (mkfifo("1to2",0600)) {
		if (errno != EEXIST) {
			perror("Eroare la crearea pipe-ului intre 1 si 2!");
			exit(4);
		}
	}

	pid_t pids[2];
	if ((pids[0] = fork()) == -1){
		perror("Eroare la crearea primului fiu!");
		exit(5);
	}

	if (pids[0] == 0) {
		int fdW,fdR;
		close(pipes[0][1]);
		close(pipes[1][0]);
		close(pipes[1][1]);

		if ((fdW = (open("1to2", O_WRONLY))) == -1) {
			perror("Eroare la deschiderea comunicatiei!");
			exit(10);
		}

		if ((fdR = (open("1to2", O_RDONLY))) == -1) {
			perror("Eroare la test1");
			exit(11);
		}

        char ch;
		while (read(pipes[0][0], &ch, sizeof(char)) > 0) {
			if (ch >= 'a' && ch <= 'f') {
				ch -= 32;
			}
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F')) {
				write(fdW, &ch, sizeof(char));
			}
		}

		close(pipes[0][0]);
		close(fdW);
		close(fdR);
        return 0;
	}
	else {
		if ((pids[1] = fork()) == -1) {
			perror("Eroare la crearea celui de-al doilea fiu!");
			exit(6);
		}

		if (pids[1] == 0) {
			close(pipes[0][0]);
			close(pipes[0][1]);
			close(pipes[1][0]);
			int fdR, fd;
			int vec[64] = { 0 };

			if ((fdR = open("1to2", O_RDONLY)) == -1) {
				perror("Eroare la deschiderea comunicatiei!");
				exit(13);
			}

            char ch;
			while (read(fdR, &ch, sizeof(char)) > 0) {
				if (ch >= '0' && ch <= '9'){
					vec[ch - 48]++;
				}
				if (ch >='A' && ch <= 'F') {
					vec[ch - 55]++;
				}
			}

			if ((fd = open("statistics.txt", O_WRONLY | O_TRUNC | O_CREAT, 0666))) {
				perror("Eroare la deschiderea fisierului statistica!");
				exit(15);
			}

            printf("Aici ar trebui sa se scrie in stats\n");

			close(fdR);
			close(fd);
			close(pipes[1][1]);
		}
		else {
			close(pipes[1][1]);
			close(pipes[0][0]);

			int fd;
			if ((fd = open(argv[1], O_RDWR)) == -1) {
				perror("Eroare la deschiderea fisierului cu informatii!");
				exit(7);
			}

            char ch;
			while (read(fd, &ch, sizeof(char)) > 0) {
				write(pipes[0][1], &ch, sizeof(char));
			}
			close(pipes[0][1]);

			int buf;
			read(pipes[1][0], &buf, sizeof(int));
            printf("%d\n", buf);

			close(pipes[1][0]);
			close(fd);

			wait(NULL);
			wait(NULL);
		}
	}

	return 0;
}
