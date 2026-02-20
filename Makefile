# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/04 16:27:31 by timurray          #+#    #+#              #
#    Updated: 2026/02/14 17:11:16 by timurray         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minirt
CC = cc
CFLAGS = -Wall -Wextra -Werror
VFLAGS = -g -O0
SFLAGS = -ggdb3 -fsanitize=address -fsanitize=leak -fsanitize=undefined
INCLUDES = -I./include -I./MLX42/include -I./libft
INCLUDES = -I. -I./MLX42/include -I./libft -I./include

SRCS = \
src/parse.c \
src/error.c \
src/main_cpu.c \
src/render_cpu2.c \
src/class/vec3.c \
src/class/sphere.c \
src/class/hittable.c \
src/class/interval.c

OBJ_DIR = obj
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)


MLX42_REPO = https://github.com/codam-coding-college/MLX42.git
MLX42_DIR  = MLX42
MLX42_BUILD = $(MLX42_DIR)/build/libmlx42.a

LIBFT_DIR  = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

MLX_FLAGS = -ldl -lglfw -pthread -lm -lz

debug ?= 0
ifeq ($(debug), 1)
	CFLAGS += $(VFLAGS)
endif
ifeq ($(debug), 2)
	CFLAGS += $(SFLAGS)
endif

all: $(NAME)

$(NAME): $(MLX42_BUILD) $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX42_BUILD) $(LIBFT) $(MLX_FLAGS) -o $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(MLX42_DIR):
	git clone $(MLX42_REPO) $(MLX42_DIR)

$(MLX42_BUILD): | $(MLX42_DIR)
	cmake -B $(MLX42_DIR)/build -S $(MLX42_DIR)
	cmake --build $(MLX42_DIR)/build -j4

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(MLX42_DIR)/build
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(MLX42_DIR)
	make fclean -C $(LIBFT_DIR)

re: fclean all

.PHONY: all clean fclean re
