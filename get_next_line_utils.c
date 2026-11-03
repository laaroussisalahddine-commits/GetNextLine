/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:38:53 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/02 19:33:46 by slaarous         ###   ########.fr       */
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
char	*get_alloc(void)
{
	char	*s;

	s = malloc(1);
	if (!s)
		return (NULL);
	s[0] = 0;
	return (s);
}

char	*copy_new_stash(char *stash, int pos, int count)
{
	char	*new_stash; 
	int		j;

	new_stash = (char *)malloc(count + 1);
	if (!new_stash)
		return (NULL);
	j = 0;
	while (j < count && stash[pos + j])
	{
		new_stash[j] = stash[pos + j];
		j++;
	}
	new_stash[j] = 0;
	return (new_stash);
}

char	*get_join(char *stash, char *buffer)
{
	char	*new_string;
	int		i;
	int		j;

	if (!stash)
	{
		stash = get_alloc();
		if (!stash)
			return (free(buffer), free(stash), NULL);
	}
	new_string = (char *)malloc(get_strlen(stash) + get_strlen(buffer) + 1);
	if (!new_string)
		return (free(buffer), NULL);
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
	return (free(stash), free(buffer), new_string);
}
