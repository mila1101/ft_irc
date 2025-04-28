# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/12 17:38:35 by eahn              #+#    #+#              #
#    Updated: 2025/04/28 17:46:13 by eahn             ###   ########.fr        #
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
OBJ_SRCS = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJ_DIR), $(OBJ_SRCS))

# Default target
all: $(NAME)

# Create object directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR) $(OBJ_DIR)client $(OBJ_DIR)commands $(OBJ_DIR)core $(OBJ_DIR)utils

# Link all objects to create executable
$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Rule for compiling each .cpp to .o
$(OBJ_DIR)%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	
re: fclean all

.PHONY: all clean fclean re
