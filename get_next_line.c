/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:35:31 by slaarous          #+#    #+#             */
/*   Updated: 2026/10/31 22:33:53 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*read_file(char *stash, int fd)
{
	char		buffer[BUFFER_SIZE];
	int			byte;
	int			i;

	while(1)
	{
		buffer = (char *)malloc(BUFFER_SIZE + 1);
		if (!buffer)
			return ;
		byte = read(fd, buffer, BUFFER_SIZE);
		if (!stash && byte <= 0)
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

char	*update_stash(char *stash)
{
	char	*new_stash;
	char	*start_line;
	int		len;

	len = 0;
	while (*stash && stash != '\n')
	{
		stash++;
		len++;
	}
	if (*stash == '\n')
	{
		stash++;
		len++;
	}
	new_stash = (char *)malloc(len + 1);
	if (!new_stash)
		return (NULL);
	len = 0;
	while(*stash)
	{
		new_stash[len++] = *stash;
		stash++;
	}
	new_stash[len] = 0;
	return (new_stash);
}

char	*ft_get_next_line(int fd)
{
	static char *stash;
	
	if (fd < 0 || BUFFER_SIZE < 0)
		return (NULL);
}