#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 4096 // This is exactly the page size , for disk I /O efficiency !
 
int main(int argc, char* argv[]) {
    int input_fd, output_fd;
    ssize_t bytes_in, bytes_out; 
    char buffer[BUF_SIZE];

    if (argc != 3) { printf ("Usage: src_file dest_file"); return 1; }

    input_fd = open(argv[1] , O_RDONLY);
    if (input_fd == -1) { perror(argv [1]); return 2; }

    int x = access(argv[2], F_OK);
    if (x == 0) {
        // fisierul deja exista
        printf("Fisierul '%s' deja exista. Doriti sa suprascrieti? y/n: ", argv[2]);
        char answer = 0;
        scanf("%c", &answer);
        if (answer == 'y') {
            output_fd = open(argv[2] , O_WRONLY | O_CREAT | O_TRUNC , 0600);
            if (output_fd == -1) { perror(argv [2]); return 3; }
            long y = lseek(output_fd, 0L, SEEK_END);
            struct stat* st = NULL;
            int z = fstat(output_fd, st);
            printf("%ld", st->st_size);
        }
        else if (answer == 'n') {
            printf("Umm.. ok");
            return 0;
        }
        else {
            printf("Invalid answer.");
            return 5;
        }
    }

    /* Process the input file a record at atime . */
    while ((bytes_in = read(input_fd, buffer, BUF_SIZE)) > 0) {
        for (int i = 0; i < bytes_in; i++) {
            char transform[4] = {};
            ssize_t bytes_to_write = 3; 
            switch (buffer[i]) {
                case '0':
                    strcpy(transform, "000");
                    break;
                case '1':
                    strcpy(transform, "001");
                    break;
                case '2':
                    strcpy(transform, "010");
                    break;
                case '3':
                    strcpy(transform, "011");
                    break;
                case '4':
                    strcpy(transform, "100");
                    break;
                case '5':
                    strcpy(transform, "101");
                    break;
                case '6':
                    strcpy(transform, "110");
                    break;
                case '7':
                    strcpy(transform, "111");
                    break;
                default:
                    transform[0] = buffer[i];
                    bytes_to_write = 1;
                    break;
            }
            bytes_out = write(output_fd, transform, bytes_to_write);
            if (bytes_out != bytes_to_write) {
                perror (" Fatal write error .");
                return 4;
            }
        }
    }

    close(input_fd);
    close(output_fd);
    return 0;
}
