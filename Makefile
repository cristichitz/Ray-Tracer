# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/04 16:27:31 by timurray          #+#    #+#              #
#    Updated: 2026/05/27 18:50:00 by timurray         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = miniRT
NAME_BONUS = miniRT_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror
VFLAGS = -g -O0
SFLAGS = -ggdb3 -fsanitize=address -fsanitize=leak -fsanitize=undefined

# MANDATORY FILES
SRCS = \
src/parse.c \
src/error.c \
src/main_cpu.c \
src/render_cpu2.c \
src/class/vec3.c \
src/class/sphere.c \
src/class/hittable.c \
src/class/plane.c \
src/class/cylinder.c \
src/class/interval.c \
src/class/material.c \
src/class/texture.c \
src/bench.c \
src/utils.c

INCLUDES = -I. -I./MLX42/include -I./libft -I./include

# BONUS FILES
BONUS_SRCS = \
src/main_bonus.c \
src/cl_util_bonus.c \
src/init_gpu_bonus.c \
src/cleanup_bonus.c \
src/utils_bonus.c \
src/view_bonus.c \
src/scene_bonus.c \
src/parse_bonus.c \
src/parse_utils_bonus.c \
src/parse_obj_bonus.c \
src/parse_world_bonus.c \
src/class/vec3_bonus.c \
src/class/sphere_bonus.c \
src/class/material_bonus.c \
src/class/plane_bonus.c \
src/class/cylinder_bonus.c \
src/rubick_build_bonus.c \
src/rubick_move_bonus.c \
src/rubick_engine_bonus.c

BONUS_INCLUDES = -I. -I./MLX42/include -I./libft -I./include -I./kernels
BONUS_CFLAGS = -DCL_TARGET_OPENCL_VERSION=300


OBJ_DIR = obj
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
BONUS_OBJS = $(BONUS_SRCS:%.c=$(OBJ_DIR)/%.o)


MLX42_REPO = https://github.com/codam-coding-college/MLX42.git
MLX42_DIR  = MLX42
MLX42_BUILD = $(MLX42_DIR)/build/libmlx42.a

LIBFT_DIR  = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

MLX_FLAGS = -ldl -lglfw -pthread -lm -lz
BONUS_LINK_FLAGS = -L./libs -lOpenCL -Wl,-rpath,'$$ORIGIN/libs' -Wl,--disable-new-dtags

MACOS_GLFW_FLAGS = -I/opt/homebrew/cellar/glfw/3.4/include/ -L/opt/homebrew/cellar/glfw/3.4/lib/
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	MLX_FLAGS += $(MACOS_GLFW_FLAGS)
endif

debug ?= 0
ifeq ($(debug), 1)
	CFLAGS += $(VFLAGS)
endif
ifeq ($(debug), 2)
	CFLAGS += $(SFLAGS)
endif

# Optimised build for benchmarking: make optimize=1
optimize ?= 0
ifeq ($(optimize), 1)
	CFLAGS += -O3 -march=native -funroll-loops -flto -DNDEBUG
endif


all: $(NAME)

$(NAME): $(MLX42_BUILD) $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX42_BUILD) $(LIBFT) $(MLX_FLAGS) -o $(NAME)

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(MLX42_BUILD) $(LIBFT) $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(BONUS_CFLAGS) $(BONUS_OBJS) $(MLX42_BUILD) $(LIBFT) $(MLX_FLAGS) $(BONUS_LINK_FLAGS) -o $(NAME_BONUS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%_bonus.o: %_bonus.c
	@mkdir -p $(@D)		
	$(CC) $(CFLAGS) $(BONUS_CFLAGS) $(BONUS_INCLUDES) -c $< -o $@

$(LIBFT):
	make -C $(LIBFT_DIR)

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
	rm -f $(NAME) $(NAME_BONUS)
	rm -rf $(MLX42_DIR)
	make fclean -C $(LIBFT_DIR)

re: fclean all

# Emit annotated Intel assembly of the hot render path (inspect render_cpu2.s)
ASM_SRC = src/render_cpu2.c src/class/vec3.c
asm:
	$(CC) -Wall -Wextra -O3 -march=native -fverbose-asm -masm=intel \
		$(INCLUDES) -S $(ASM_SRC)
	@echo "wrote render_cpu2.s vec3.s"

.PHONY: all bonus clean fclean re asm
