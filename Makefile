# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/04 16:27:31 by timurray          #+#    #+#              #
#    Updated: 2026/06/14 14:34:58 by cdohanic         ###   ########.fr        #
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
mandatory/src/error.c \
mandatory/src/main_cpu.c \
mandatory/src/render_cpu.c \
mandatory/src/utils.c \
mandatory/src/quality.c \
mandatory/src/control/cam_control.c \
mandatory/src/control/obj_control.c \
mandatory/src/control/render_mode.c \
mandatory/src/control/move_object.c \
mandatory/src/control/resize.c \
mandatory/src/control/cam_view.c \
mandatory/src/class/vec/vec3.c \
mandatory/src/class/vec/vec3_2.c \
mandatory/src/class/vec/vec3_3.c \
mandatory/src/class/sphere.c \
mandatory/src/class/hittable.c \
mandatory/src/class/ray.c \
mandatory/src/class/plane.c \
mandatory/src/class/quad/quad.c \
mandatory/src/class/quad/aabb.c \
mandatory/src/class/cylinder/cylinder.c \
mandatory/src/class/cylinder/cylinder_body.c \
mandatory/src/class/cylinder/cylinder_caps.c \
mandatory/src/class/interval.c \
mandatory/src/class/material/metal.c \
mandatory/src/class/material/light.c \
mandatory/src/class/material/lambertian.c \
mandatory/src/class/texture.c \
mandatory/src/class/quaternion.c \
mandatory/src/class/box.c \
mandatory/src/parse/checks.c \
mandatory/src/parse/getters.c \
mandatory/src/parse/parse_utils.c \
mandatory/src/parse/parse.c \
mandatory/src/parse/process.c \
mandatory/src/parse/scene.c \
mandatory/src/parse/set_lights.c \
mandatory/src/parse/set_obj.c \
mandatory/src/parse/set_obj_2.c \
mandatory/src/parse/set_params.c \
mandatory/src/parse/set_params_2.c \
mandatory/src/render_direct.c \
mandatory/src/random.c

INCLUDES = -I. -I./MLX42/include -I./libft -I./mandatory/include

# BONUS FILES
BONUS_SRCS = \
bonus/src/main_bonus.c \
bonus/src/utils_bonus.c \
bonus/src/control/controls_bonus.c \
bonus/src/control/view_bonus.c \
bonus/src/cl/cl_util_bonus.c \
bonus/src/cl/cl_info_bonus.c \
bonus/src/cl/cl_select_bonus.c \
bonus/src/cl/cl_program_bonus.c \
bonus/src/cl/cl_release_bonus.c \
bonus/src/gpu/init_gpu_bonus.c \
bonus/src/gpu/render_gpu_bonus.c \
bonus/src/gpu/cleanup_bonus.c \
bonus/src/scene/scene_bonus.c \
bonus/src/scene/scene_build_bonus.c \
bonus/src/parse/parse_bonus.c \
bonus/src/parse/parse_utils_bonus.c \
bonus/src/parse/parse_obj_bonus.c \
bonus/src/parse/parse_world_bonus.c \
bonus/src/class/vec3_bonus.c \
bonus/src/class/vec3_2_bonus.c \
bonus/src/class/vec3_3_bonus.c \
bonus/src/class/sphere_bonus.c \
bonus/src/class/material_bonus.c \
bonus/src/class/material_preset_bonus.c \
bonus/src/class/plane_bonus.c \
bonus/src/class/cylinder_bonus.c \
bonus/src/physics/quat_bonus.c \
bonus/src/physics/quat_2_bonus.c \
bonus/src/physics/body_bonus.c \
bonus/src/physics/body_place_bonus.c \
bonus/src/physics/physics_bonus.c \
bonus/src/physics/physics_step_bonus.c \
bonus/src/physics/physics_input_bonus.c \
bonus/src/physics/collide_bonus.c \
bonus/src/physics/collide_box_bonus.c \
bonus/src/physics/collide_sat_bonus.c \
bonus/src/physics/collide_ball_bonus.c \
bonus/src/physics/collide_sphere_bonus.c \
bonus/src/physics/collide_room_bonus.c \
bonus/src/bvh/bvh_bounds_bonus.c \
bonus/src/bvh/bvh_build_bonus.c \
bonus/src/portal/portal_bonus.c \
bonus/src/portal/portal_hit_bonus.c \
bonus/src/portal/portal_warp_bonus.c \
bonus/src/render_out_bonus.c


BONUS_INCLUDES = -I. -I./MLX42/include -I./libft -I./bonus/include -I./bonus/kernels
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
BONUS_LINK_FLAGS = -L./bonus/libs -lOpenCL -Wl,-rpath,'$$ORIGIN/bonus/libs' -Wl,--disable-new-dtags

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

$(OBJ_DIR)/mandatory/%.o: mandatory/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/bonus/%.o: bonus/%.c
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
ASM_SRC = mandatory/src/render_cpu.c mandatory/src/class/vec3.c
asm:
	$(CC) -Wall -Wextra -O3 -march=native -fverbose-asm -masm=intel \
		$(INCLUDES) -S $(ASM_SRC)
	@echo "wrote render_cpu2.s vec3.s"

.PHONY: all bonus clean fclean re asm
