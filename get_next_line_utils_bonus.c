#include "get_next_line_bonus.h"

int	get_search_bonus(char *s, int c)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i] && s[i] != c)
		i++;
	if (s[i] == c)
		return (1);
	return (0);
}

char	*get_alloc_bonus(void)
{
	char	*s;

	s = (char *)malloc(1);
	if (!s)
		return (NULL);
	s[0] = '\0';
	return (s);
}

size_t	get_strlen_bonus(char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}
char	*get_new_stash_bonus(char *stash, size_t start, size_t len)
{
	char	*new;
	size_t	i;

	new = (char *)malloc(len + 1);
	if (!new)
		return (NULL);
	i = 0;
	while(stash[start])
	{
		new[i++] = stash[start];
		start++;
	}
	new[i] = 0;
	return (new);
}

char	*get_join_bonus(char *stash, char *buffer)
{
	char	*new_string;
	size_t	i;
	size_t	j;

	if (!stash)
	{
		stash = get_alloc_bonus();
		if (!stash)
			return (free(buffer), NULL);
	}
	new_string = (char *)malloc(get_strlen_bonus(stash) + get_strlen_bonus(buffer) + 1);
	if (!new_string)
		return (free(buffer), NULL);
	i = 0;
	while (stash[i++])
		new_string[i] = stash[i];
	j = -1;
	while (buffer[++j])
		new_string[i++] = buffer[j];
	new_string[i] = 0;
	return(free(stash), free(buffer), new_string);
}
