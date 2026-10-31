/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:38:53 by slaarous          #+#    #+#             */
/*   Updated: 2026/10/31 22:05:31 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_strchr(char *str, int c)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != c)
		i++;
	if (str[i] == c)
		return (&str[i]);
	return (NULL);
}

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
	void	*s;
	int		i;

	if (size != 0 && nmemb > SIZE_MAX / size)
		return (NULL);
	total = nmemb * size;
	s = malloc(total);
	if (!s)
		return (NULL);
	i = 0;
	while (i < nmemb)
	{
		(char *)s[i] = 0;
		i++;
	}
	return (s);
}

char	*get_join(char *stash, char *buffer)
{
	char	*new;
	int		i;
	int		j;

	if (!stash)
	{
		stash = ft_calloc(1);
		if (!stash)
			return (NULL);
	}
	new = (char *)malloc(get_strlen(stash) + get_strlen(buffer) + 1)
	if (!new)
		return (NULL);
	i = -1;
	while (stash[++i])
		new[i] = stash[i];
	j = 0;
	while (buffer[j])
		new[i++] = buffer[j++];
	new[i] = '\0';
	free(stash);
	return (new);
}
