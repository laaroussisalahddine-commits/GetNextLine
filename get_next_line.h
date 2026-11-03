/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:39:16 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/02 17:42:15 by slaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# include <unistd.h>
# include <stdlib.h>


char	*get_strchr(char *str, int c);
int		get_strlen(char *str);
char	*get_alloc(void);
char	*get_join(char *stash, char *buffer);
char	*read_file(char *stash, int fd);
char	*extract_line(char *stash);
char	*update_stash(char *stash, char **line);
char	*get_next_line(int fd);
char	*copy_new_stash(char *stash, int pos, int count);

#endif