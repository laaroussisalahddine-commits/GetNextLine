#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>

int main(int ac, char **av)
{
	(void)ac;
	
	int	fd = open(av[1], O_RDONLY);
	char *s;
	while ((s = get_next_line(fd)))
	{
		printf ("%s\n", s);
		free (s);
	}
	close(fd);
}
