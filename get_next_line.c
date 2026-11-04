/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:35:31 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/04 13:52:56 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"


char	*get_next_line(int fd)
{
	static char *stash;
	char		*stock;
	char		*line;
	
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	stock = read_file(stash, fd);
	if (!stock)
	{
		if (stash)
			free(stash);
		stash = NULL;
		return (NULL);
	}
	stash = stock;
	line  = extract_line(stash);
	if (!line)
	{
		free(stash);
		stash = NULL;
		return (NULL);
	}
	stash = update_stash(stash, &line);
	return (line);
}

char	*read_file(char *stash, int fd)
{
	char			*buffer;
	ssize_t			byte;

	byte = 1;
	while(byte != 0)
	{
		buffer = malloc(BUFFER_SIZE + (size_t)1); //search of why size_t cast;
		if (!buffer)
			return (NULL);
		byte = read(fd, buffer, BUFFER_SIZE);
		if ((!stash && byte == 0) || byte == -1)
		{
			free(buffer);
			return(NULL); // add free (stash);
		}
		buffer[byte] = '\0';
		stash = get_join(stash, buffer);
		if (get_search(stash, '\n'))
			break;
	}
	return (stash);
}

char	*extract_line(char *stash)
{
	char	*line;
	size_t		i;

	if (!stash)
		return (NULL);
	i = 0;
	while(stash[i] && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	line = (char *)malloc(i + 1);
	if (!line)
		return (NULL);
	i = 0;
	while(stash[i] && stash[i] != '\n')
	{
		line[i] = stash[i];
		i++;
	}
	if (stash[i] == '\n')
		line[i++] = '\n';
	line[i] = 0;
	return (line);
}

char	*update_stash(char *stash, char **line)
{
	char	*new_stash;
	size_t		i;
	size_t		j;

	i = get_strlen(*line);
	if (!stash[i])
		return (free(stash), NULL);
	j = 0;
	while (stash[i + j])
		j++;
	new_stash = copy_new_stash(stash, i, j);
	if (!new_stash)
	{
		free(*line);
		*line = NULL;
		return (free(stash), NULL);
	}
	free(stash);
	return (new_stash);
}

