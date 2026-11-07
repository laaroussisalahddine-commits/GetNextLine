#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 9999
# endif

# ifndef MAX_INDEX
#  define MAX_INDEX 1024
# endif

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>

/*----main function----*/
char	*get_next_line_bonus(int fd);
char	*update_stash_bonus(char *stash, char **line);
char	*extract_line_bonus(char *stash);
char	*read_file_bonus(char *stash, int fd);

/*----helper function----*/
char	*get_join_bonus(char *stash, char *buffer);
char	*get_new_stash_bonus(char *stash, size_t start, size_t len);
size_t	get_strlen_bonus(char *s);
char	*get_alloc_bonus(void);
int		get_search_bonus(char *s, int c);

#endif