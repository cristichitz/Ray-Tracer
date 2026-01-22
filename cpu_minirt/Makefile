NAME = minirt_cpu
CC = cc
CFLAGS = -Wall -Wextra -Werror

# Update these to match where you saved the CPU files
SRCS = main_cpu.c render_cpu.c
OBJS = $(SRCS:.c=.o)

# Assumes rt_cpu.h is in the current directory or ./include
INCLUDES = -I. -I../../minilibx

MLX_PATH = ../../minilibx/
MLX_NAME = libmlx.a
MLX = $(MLX_PATH)$(MLX_NAME)

# Removed -lOpenCL
MLX_FLAGS = -L$(MLX_PATH) -lmlx -lXext -lX11 -lm -lz

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