#include "get_next_line_bonus.h"
#include <stdio.h>
#include <fcntl.h>

int	main(int ac, char **av)
{
	(void)ac;
	int		fd1;
	int		fd2;
	int		i;
	char	*s;
	char	*p;

	if (ac == 1)
		return (printf("Usage: %s file1 file2 ....\n", av[0]), 1);
	fd1 = open(av[1], O_RDONLY);
	fd2 = open(av[2], O_RDONLY);
	if (fd1 < 0 || fd2 < 0)
	{
		perror("open");
		if (fd1 >= 0)
			close(fd1);
		if (fd2 >= 0)
			close(fd2);
		return (1);
	}
	i = 1;
	while (1)
	{
		s = get_next_line_bonus(fd1);
		p = get_next_line_bonus(fd2);
		if (!s && !p)
			break ;
		if (s)
		{
			printf("FD1 line %d: %s", i, s);
			free(s);
		}
		if (p)
		{
			printf("FD2 line %d: %s", i, p);
			free(p);
		}
		i++;
	}
	close(fd1);
	close(fd2);
	return (0);
}