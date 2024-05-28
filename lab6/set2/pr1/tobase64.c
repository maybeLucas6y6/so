#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 3

char to_base_64(int x) {
    if (x >= 0 && x < 26) {
        return 'A' + x;
    }
    else if (x >= 26 && x < 52) {
        return 'a' + (x - 26);
    }
    else if (x >= 53 && x < 62) {
        return '0' + (x - 53);
    }
    else if(x == 62) {
        return '+';
    }
    else if(x == 63) {
        return '/';
    }
    else {
        return -1;
    }
}
 
int main(int argc, char* argv[]) {
    int input_fd, output_fd;
    ssize_t bytes_in, bytes_out; 
    char buffer[BUF_SIZE];

    if (argc != 3) {
        printf("Usage: src_file dest_file");
        return 1;
    }

    input_fd = open(argv[1] , O_RDONLY);
    if (input_fd == -1) {
        perror(argv [1]);
        return 2;
    }

    if (access(argv[2], F_OK) == 0 && 0) {
        // fisierul deja exista
        printf("Fisierul '%s' deja exista. Doriti sa suprascrieti? y/n: ", argv[2]);
        char answer = 0;
        scanf("%c", &answer);
        if (answer == 'y') {
            output_fd = open(argv[2] , O_WRONLY | O_CREAT | O_TRUNC , 0600);
            if (output_fd == -1) {
                perror(argv [2]);
                return 3;
            }

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
            return 4;
        }
    }
    else {
        output_fd = open(argv[2] , O_WRONLY | O_CREAT | O_TRUNC , 0600);
        if (output_fd == -1) {
            perror(argv [2]);
            return 3;
        }
    }

    /* Process the input file a record at atime . */
    while ((bytes_in = read(input_fd, buffer, BUF_SIZE)) > 0) {
        printf ("bytes read %ld\n", bytes_in);
        int tr = 0;
        char transform[4] = {};

        for (int i = 0; i < bytes_in; i++) {
            tr = tr << 8;
            tr += buffer[i];
            printf("%c %d, ", buffer[i], tr);
        }

        printf("----\n");
        
        int first = tr >> 18;
        transform[0] = to_base_64(first);
        printf("%d ", first);

        int second = tr >> 12;
        second = second & 63;
        transform[1] = to_base_64(second);
        printf("%d ", second);

        int third = tr >> 6;
        third = third & 63;
        transform[2] = to_base_64(third);
        printf("%d ", third);

        int fourth = tr & 63;
        transform[3] = to_base_64(fourth);
        printf("%d ", fourth);

        for (int i = 0; i < 4; i++) {
            printf("%c", transform[i]);
        }
        printf("\n");

        bytes_out = write(output_fd, transform, 4);
        if (bytes_out != 4) {
            perror ("Fatal write error .");
            return 5;
        }
    }

    close(input_fd);
    close(output_fd);
    return 0;
}
