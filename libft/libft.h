/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:35:15 by timurray          #+#    #+#             */
/*   Updated: 2025/12/13 15:43:50 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

# include <limits.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_vec
{
	unsigned char	*memory;
	size_t			elem_size;
	size_t			alloc_size;
	size_t			len;
}					t_vec;

int					ft_printf(const char *s, ...);
int					ft_printfd(int fd, const char *s, ...);
ssize_t				ft_putchar_fd(char c, int fd);
int					char_handler(const int c);
ssize_t				func_select(const char key, va_list *args);
ssize_t				ft_putstr_fd(const char *s, int fd);
ssize_t				str_handler(const char *s);
ssize_t				num_handler(const long num, const char *s);
ssize_t				unum_handler(const unsigned long long num, const char *s);
ssize_t				ptr_handler(const void *ptr, const char *s);
char				*ft_read_line_check(char *read_line);
char				*ft_read_line(char *read_line, int fd);
int					ft_get_n_index(const char *s, int c);
char				*ft_n_exist(const char *s, int c);
char				*get_next_line(int fd);
char				*ft_strjoin(char const *s1, char const *s2);
void				*ft_memcpy(void *dest, const void *src, size_t n);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strdup(const char *s);
int					ft_atoi(const char *nptr);
long				ft_atol(const char *nptr);
void				ft_bzero(void *s, size_t n);
void				*ft_calloc(size_t nmemb, size_t size);
int					ft_isalnum(int c);
int					ft_isalpha(int c);
int					ft_isascii(int c);
int					ft_isdigit(int c);
int					ft_isprint(int c);
int					ft_isspace(char c);
char				*ft_itoa(int n);
void				*ft_memchr(const void *s, int c, size_t n);
int					ft_memcmp(const void *s1, const void *s2, size_t n);
void				*ft_memcpy(void *dest, const void *src, size_t n);
void				*ft_memmove(void *dest, const void *src, size_t n);
void				*ft_memset(void *s, int c, size_t n);
void				ft_putendl_fd(char *s, int fd);
void				ft_putnbr_fd(int n, int fd);
char				**ft_split(char const *s, char c);
char				*ft_strchr(const char *s, int c);
char				*ft_strdup(const char *s);
void				ft_striteri(char *s, void (*f)(unsigned int, char *));
char				*ft_strjoin(char const *s1, char const *s2);
size_t				ft_strlcat(char *dst, const char *src, size_t dstsize);
size_t				ft_strlcpy(char *dst, const char *src, size_t dstsize);
size_t				ft_strlen(const char *s);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
int					ft_strncmp(const char *s1, const char *s2, size_t n);
char				*ft_strnstr(const char *big, const char *little,
						size_t len);
char				*ft_strrchr(const char *s, int c);
char				*ft_strtrim(char const *s1, char const *set);
char				*ft_substr(char const *s, unsigned int start, size_t len);
int					ft_tolower(int c);
int					ft_toupper(int c);
void				ft_free_split(char **arr);
int					ft_atoi_check(const char *nptr, int *num);
int					ft_strcmp(const char *s1, const char *s2);
int					ft_vec_new(t_vec *dst, size_t init_len, size_t elem_size);
int					ft_vec_resize(t_vec *src, size_t target_len);
void				ft_vec_free(t_vec *src);
int					ft_vec_push(t_vec *dst, void *src);
int					ft_vec_pop(void *dst, t_vec *src);
void				*ft_vec_get(t_vec *src, size_t index);
int					ft_vec_from(t_vec *dst, void *src, size_t len,
						size_t elem_size);
void				*vec_find(t_vec *src, bool (*f)(void *));

#endif