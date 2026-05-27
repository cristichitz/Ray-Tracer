# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/31 16:30:13 by timurray          #+#    #+#              #
#    Updated: 2025/11/10 15:23:15 by timurray         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror -I.

SRCS = \
	base/ft_calloc.c base/ft_isdigit.c base/ft_memcmp.c \
	base/ft_putendl_fd.c base/ft_strchr.c base/ft_strlcat.c \
	base/ft_strncmp.c base/ft_substr.c base/ft_isalnum.c  \
	base/ft_isprint.c base/ft_memcpy.c base/ft_putnbr_fd.c \
	base/ft_strdup.c base/ft_strlcpy.c base/ft_strnstr.c  \
	base/ft_tolower.c base/ft_atoi.c base/ft_isalpha.c \
	base/ft_itoa.c base/ft_memmove.c  base/ft_atol.c\
	base/ft_striteri.c base/ft_strrchr.c base/ft_isspace.c \
	base/ft_toupper.c base/ft_bzero.c base/ft_isascii.c \
	base/ft_memchr.c base/ft_memset.c base/ft_split.c \
	base/ft_strjoin.c base/ft_strmapi.c base/ft_strtrim.c \
	base/ft_free_split.c base/ft_atoi_check.c base/ft_strlen.c \
	base/ft_strcmp.c \
	get_next_line/get_next_line.c \
	printf/ft_printf.c printf/ft_printf_char_utils.c \
	printf/ft_printf_num_utils.c \
	vector/allocation.c vector/operations.c vector/search.c \

OBJS = $(SRCS:.c=.o)

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	ar -rcs $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
