#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    char* args[] = {
        "find",
        "/home/luca/facultate/an1/sem2/so/",
        "(",
        "-name",
        "*.c",
        "-or",
        "-name",
        "*.sh",
        ")",
        "-printf",
        "%p : %k KB\n",
        NULL
    };

    if (-1 == (pid=fork()))
    {
        perror("Eroare la fork");  exit(1);
    }

    if (pid == 0)
    {
        execv("/bin/find", args);

        perror("Eroare la exec");
        exit(10);
    }

    /* (Doar în procesul părinte) Acum cercetez cum s-a terminat procesul fiu. */

    wait(&status);

    if( WIFEXITED(status) )
    {
        switch( WEXITSTATUS(status) )
        {
            case 10:  printf("Comanda find nu a putut fi executata... (apelul exec a esuat).\n");  break;
            case  0:  printf("Comanda find a fost executata cu succes!\n");  break;
            default:  printf("Comanda find a fost executata, dar a esuat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status) );
        }
    }
    else
    {
        printf("Comanda find a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status) );
    }

    return 0;
}
