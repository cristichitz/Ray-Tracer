NAME = minirt
CC = cc
CFLAGS = -Wall -Wextra -Werror

OBJ_DIR = obj/

SRCS = src/main.c src/gpu_setup.c src/math/vec3.c src/math/color.c

OBJS = $(SRCS:%.c=$(OBJ_DIR)%.o)
INCLUDES = -I./include -I../MLX42/include

MLX_PATH = ../MLX42/
MLX_BUILD_PATH = $(MLX_PATH)/build
MLX_LIB = $(MLX_BUILD_PATH)/libmlx42.a

MLX_FLAGS = -L$(MLX_BUILD_PATH) -L./libs -lmlx42 -ldl -lglfw -pthread -lm -lz -lOpenCL -Wl,-rpath,'$$ORIGIN/libs' -Wl,--disable-new-dtags

all: $(MLX_LIB) $(NAME)

$(MLX_LIB):
	@echo "Building MLX42..."
	@cmake $(MLX_PATH) -B $(MLX_BUILD_PATH)
	@cmake --build $(MLX_BUILD_PATH) -j4

$(OBJ_DIR)%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
			 $(CC) $(CFLAGS) $(OBJS) $(MLX_FLAGS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)
	@rm -rf $(MLX_BUILD_PATH)	

fclean: clean
				rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re	