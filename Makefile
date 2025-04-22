# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: msoklova <msoklova@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/12 17:38:35 by eahn              #+#    #+#              #
#    Updated: 2025/04/22 15:46:22 by msoklova         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17 -I.

NAME = ircserv

CLIENT_DIR = ./client/
COMMANDS_DIR = ./commands/
CORE_DIR = ./core/
UTILS_DIR = ./utils/
OBJ_DIR = ./obj/

CLIENT_SRCS = Client.cpp
COMMANDS_SRCS = CommandHandler.cpp
CORE_SRCS = Server.cpp SocketHandler.cpp main.cpp
UTILS_SRCS = InputParser.cpp Logger.cpp

# Full paths to sources
SRCS = $(addprefix $(CLIENT_DIR), $(CLIENT_SRCS)) \
       $(addprefix $(COMMANDS_DIR), $(COMMANDS_SRCS)) \
       $(addprefix $(CORE_DIR), $(CORE_SRCS)) \
       $(addprefix $(UTILS_DIR), $(UTILS_SRCS))


# Object files mapped to OBJ_DIR
OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

# Replace directory prefix to obj/
# (e.g., ./core/Server.cpp → ./obj/core/Server.o)
OBJ_PATHS = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))

# Default target
all: $(NAME)

# Create object directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)client
	@mkdir -p $(OBJ_DIR)commands
	@mkdir -p $(OBJ_DIR)core
	@mkdir -p $(OBJ_DIR)utils

# Rule for compiling each .cpp to .o
$(OBJ_DIR)%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects to create executable
$(NAME): $(OBJ_PATHS)
	$(CC) $(CFLAGS) $(OBJ_PATHS) -o $(NAME)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
