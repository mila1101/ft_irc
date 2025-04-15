# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: msoklova <msoklova@student.42heilbronn.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/12 17:38:35 by eahn              #+#    #+#              #
#    Updated: 2025/04/15 16:00:38 by msoklova         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17 -I.

NAME = ircserv

SRC_DIR = ./core/
UTILS_DIR = ./utils/
OBJ_DIR = ./obj/

SRCS = Server.cpp SocketHandler.cpp
UTIL_SRCS = Logger.cpp

NEW_SRCS = $(addprefix $(SRC_DIR), $(SRCS)) $(addprefix $(UTILS_DIR), $(UTIL_SRCS))
OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o)) $(addprefix $(OBJ_DIR), $(UTIL_SRCS:.cpp=.o))

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(UTILS_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
