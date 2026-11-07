#include "get_next_line_bonus.h"

char	*get_next_line_bonus(int fd)
{
	static char	*stash[MAX_INDEX];
	char		*stock;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0
		|| !(fd >= 0 && fd < MAX_INDEX))
		return (NULL);
	stock = read_file_bonus(stash[fd], fd);
	if (!stock)
	{
		stash[fd] = NULL;
		return (NULL);
	}
	stash[fd] = stock;
	line = extract_line_bonus(stash[fd]);
	if (!line)
	{
		free(stash[fd]);
		stash[fd] = NULL;
		return (NULL);
	}
	stash[fd] = update_stash_bonus(stash[fd], &line);
	return (line);
}

char	*read_file_bonus(char *stash, int fd)
{
	char		*buffer;
	ssize_t		byte;

	byte = 1;
	while (!get_search_bonus(stash, '\n') && byte != 0)
	{
		buffer = malloc((size_t)BUFFER_SIZE + 1);
		if (!buffer)
			return (free(stash), buffer);
		byte = read(fd, buffer, BUFFER_SIZE);
		if ((!stash && byte == 0) || byte == -1)
		{
			free(buffer);
			return (free(stash), NULL);
		}
		buffer[byte] = 0;
		stash = get_join_bonus(stash, buffer);
	}
	return (stash);
}

char	*update_stash_bonus(char *stash, char **line)
{
	char	*new_stash;
	size_t	len_line;
	size_t	len_new_stash;

	len_line = get_strlen_bonus(*line);
	if (!stash[len_line])
		return (free(stash), NULL);
	len_new_stash = 0;
	while (stash[len_line + len_new_stash])
		len_new_stash++;
	new_stash = get_new_stash_bonus(stash, len_line, len_new_stash);
	if (!new_stash)
	{
		free(*line);
		*line = NULL;
		return (free(stash), NULL);
	}
	free(stash);
	return (new_stash);
}

char	*extract_line_bonus(char *stash)
{
	char	*line;
	size_t	len;
	size_t	i;

	len = 0;
	while (stash[len] && stash[len] != '\n')
		len++;
	if (stash[len] == '\n')
		len++;
	line = (char *)malloc(len + 1);
	if (!line)
		return (NULL);
	i = -1;
	while (stash[++i] && stash[i] != '\n')
		line[i] = stash[i];
	if (stash[i] == '\n')
		line[i++] = '\n';
	line[i] = 0;
	return (line);
}
