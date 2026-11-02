/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:38:53 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/01 17:34:33 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int		get_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	*get_calloc(size_t nmemb, size_t size)
{
	size_t	total;
	char	*s;
	size_t	i;

	if (size != 0 && nmemb > (size_t) - 1 / size)
		return (NULL);
	total = nmemb * size;
	s = malloc(total);
	if (!s)
		return (NULL);
	i = 0;
	while (i < nmemb)
		s[i++] = 0;
	return (s);
}

char	*get_join(char *stash, char *buffer)
{
	char	*new_string;
	int		i;
	int		j;

	if (!stash)
	{
		stash = get_calloc(1, 1);
		if (!stash)
			return (free(buffer), NULL);
	}
	new_string = (char *)malloc(get_strlen(stash) + get_strlen(buffer) + 1);
	if (!new_string)
		return (NULL);
	i = 0;
	while (stash[i])
	{
		new_string[i] = stash[i];
		i++;
	}	
	j = 0;
	while (buffer[j])
		new_string[i++] = buffer[j++];
	new_string[i] = '\0';
	return (free(stash), free(buffer),new_string);
}
