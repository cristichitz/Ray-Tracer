NAME = minirt

SRCS = \
src/main.c \
src/gpu_setup.c \
src/math/vec3.c \
src/math/color.c

OBJ_DIR = obj
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -DCL_TARGET_OPENCL_VERSION=200
INCLUDES = -I./include -I./MLX42/include

VFLAGS = -g
SFLAGS = -ggdb3 -fsanitize=address -fsanitize=leak -fsanitize=undefined

debug ?= 0
ifeq ($(debug), 1)
	CFLAGS := $(CFLAGS) $(VFLAGS)
endif
ifeq ($(debug), 2)
	CFLAGS := $(CFLAGS) $(SFLAGS)
endif

MLX42_REPO = https://github.com/codam-coding-college/MLX42.git
MLX42_DIR  = MLX42
MLX42_BUILD = $(MLX42_DIR)/build/libmlx42.a

LDFLAGS = -L./libs -Wl,-rpath,'$$ORIGIN/libs'
LDLIBS  = -ldl -lglfw -pthread -lm -lz -lOpenCL

all: $(NAME)

$(NAME): $(MLX42_BUILD) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX42_BUILD) $(LDFLAGS) $(LDLIBS) -o $(NAME)

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

fclean: clean
	rm -f $(NAME)
	rm -rf $(MLX42_DIR)

re: fclean all

.PHONY: all clean fclean re
