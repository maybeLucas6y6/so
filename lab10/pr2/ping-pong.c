#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>

int schimba_flag(int flag_fd, char val)
{
    if (write(flag_fd, &val, 1) == -1) { perror("eroare la scriere"); return 4; }
    if (lseek(flag_fd, 0L, SEEK_SET) == -1) { perror("eroare la repozitionare"); return 4; };
    return 0;
}

int read_flag(int flag_fd, char* ch)
{
    if (read(flag_fd, ch, 1) == -1) { perror("eroare la read"); return 3; }
    if (lseek(flag_fd, 0L, SEEK_SET) == -1) { perror("eroare la repozitionare"); return 4; };
    return 0;
}

int get_next_line(const char* str, int* current)
{
    if (*current == strlen(str))
    {
        return -1;
    }
    
    while (str[*current] != '\n' || str[*current] != '\0')
    {
        (*current)++;
    }
    
    return 0;
}

int dialog_tata(int flag_fd)
{
    int fd = open("tata.txt" , O_RDONLY);
    if (fd == -1) { perror("tata.txt"); return 3; }
    char buf[4096] = {};
    if (read(fd, buf, 4096) == -1) { perror("eroare la read"); return 3; }
    int current = 0;

	// Scrieți două bucle imbricate, conform indicațiilor cuprinse în diagramă!
    char flag = 0;
    while (flag != 2) {
        if (read_flag(flag_fd, &flag) != 0)
        {
            return 1;
        }
        printf("s-a citit %c\n", flag);
        printf("se asteapta tatal\n");

        if (flag == '0')
        {
            // randul tatalui
            char line[1024] = {};
            int old = current;
            strncpy(line, buf + current, (get_next_line(buf, &current), current - old));

            printf("%s", line);
            schimba_flag(flag_fd, '1');
        }
        sleep(1);
    }

    close(fd);
    return 0;
}

int dialog_fiu(int flag_fd)
{
    int fd = open("fiu.txt" , O_RDONLY);
    if (fd == -1) { perror("fiu.txt"); return 3; }
    char buf[4096] = {};
    if (read(fd, buf, 4096) == -1) { perror("eroare la read"); return 3; }
    int current = 0;

	// Scrieți două bucle imbricate, conform indicațiilor cuprinse în diagramă!
    char flag = 0;
    while (flag != 2) {
        if (read_flag(flag_fd, &flag) != 0)
        {
            return 1;
        }
        printf("s-a citit %c\n", flag);
        printf("se asteapta fiul\n");

        if (flag == '1')
        {
            // randul tatalui
            char line[1024] = {};
            int old = current;
            strncpy(line, buf + current, (get_next_line(buf, &current), current - old));

            printf("%s", line);
            schimba_flag(flag_fd, '0');
        }
        sleep(1);
    }

    close(fd);
    return 0;
}

int main()
{
	pid_t pid_fiu;

    int flag_fd = open("flag.bin" , O_RDWR | O_CREAT | O_TRUNC , 0600);
    if (flag_fd == -1) { perror("flag.bin"); return 3; }
    if (schimba_flag(flag_fd, '0') != 0) return 5;

	if ((pid_fiu = fork()) == -1)
	{
		perror("Eroare la fork");  return 1;
	}

	if(pid_fiu == 0)
	{
		printf("\n[P1] Procesul fiu, cu PID-ul: %d.\n", getpid());
		dialog_fiu(flag_fd);
	}
	else
	{
		printf("\n[P0] Procesul tata, cu PID-ul: %d.\n", getpid());
		dialog_tata(flag_fd);
	}

	printf("Sfarsitul executiei procesului %s.\n\n", pid_fiu == 0 ? "fiu" : "parinte" );
    close(flag_fd);
	return 0;
}
