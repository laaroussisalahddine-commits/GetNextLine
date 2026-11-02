/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:35:31 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/01 17:39:58 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*read_file(char *stash, int fd)
{
	char		*buffer;
	int			byte;

	byte = 1;
	while(byte != 0)
	{
		buffer = malloc(BUFFER_SIZE + 1);
		if (!buffer)
			return (NULL);
		byte = read(fd, buffer, BUFFER_SIZE);
		if ((!stash && byte == 0) || byte == -1)
		{
			free(buffer);
			return(NULL);
		}
		buffer[byte] = '\0';
		stash = get_join(stash, buffer);
	}
	return (stash);
}

char	*extract_line(char *stash)
{
	char	*line;
	int		i;

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
	int		i;
	int		j;

	i = get_strlen(*line);
	if (!stash[i])
		return (free(stash), NULL);
	j = 0;
	while (stash[i + j])
		j++;
	new_stash = malloc(sizeof(char) * (j + 1));
	if (!new_stash)
	{	
		free(*line);
		line = NULL;
		return (free(stash), NULL);
	}
	j = -1;
	while (stash[i + ++j])
		new_stash[j] = stash[i + j];
	new_stash[j] = '\0';
	free(stash);
	return (new_stash);
}

char	*get_next_line(int fd)
{
	static char *stash;
	char		*stock;
	char		*line;
	
	if (fd < 0 || BUFFER_SIZE < 1)
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
