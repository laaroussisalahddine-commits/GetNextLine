/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaarous <slaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 19:39:16 by slaarous          #+#    #+#             */
/*   Updated: 2026/11/01 20:54:34 by slaarous         ###   ########.fr       */
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
void	*get_calloc(size_t nmemb, size_t size);
char	*get_join(char *stash, char *buffer);
char	*read_file(char *stash, int fd);
char	*extract_line(char *stash);
char	*update_stash(char *stash, char **line);
char	*get_next_line(int fd);

#endif