NAME = minirt
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = src/main.c src/gpu_setup.c src/math/vec3.c src/math/color.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -I./include -I../minilibx

MLX_PATH = ../minilibx/
MLX_NAME = libmlx.a
MLX = $(MLX_PATH)$(MLX_NAME)

MLX_FLAGS = -L$(MLX_PATH) -L./libs -lmlx -lXext -lX11 -lm -lz -lOpenCL -Wl,-rpath,'$$ORIGIN/libs' -Wl,--disable-new-dtags

all: $(MLX) $(NAME)

$(MLX):
		@make -C $(MLX_PATH)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
			 $(CC) $(CFLAGS) $(OBJS) $(MLX_FLAGS) -o $(NAME)

clean:
				rm -f $(OBJS)
				@make -C $(MLX_PATH) clean

fclean: clean
				rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re