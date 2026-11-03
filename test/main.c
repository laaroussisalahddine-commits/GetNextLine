#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>

int main(int ac, char **av)
{
	(void)ac;
	int	i;
	
	int	fd = open(av[1], O_RDONLY);
	char *s;
	i = 1;
	while ((s = get_next_line(fd)))
	{
		printf ("line %d : %s\n", i, s);
		free (s);
		i++;
	}
	close(fd);
}
