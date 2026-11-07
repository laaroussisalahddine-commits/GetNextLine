#include "get_next_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd[2];
    char *line;

    if (pipe(fd) == -1)
        return (1);

    write(fd[1], "Hello\nRemaining", 15);
    close(fd[1]);

    line = get_next_line(fd[0]);
    printf("First: %s", line);
    free(line);

    close(fd[0]);

    line = get_next_line(fd[0]);
    printf("Second: %s\n", line ? line : "NULL");
    free(line);

    //executed : cc -Wall -Wextra -Werror -g -fsanitize=address -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c 
    return (0);
}