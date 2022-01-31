# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/28 18:43:03 by fhamel            #+#    #+#              #
#    Updated: 2022/01/31 15:07:25 by pnielly          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

################################################################################
#####                              FILES VARS                              #####
################################################################################

NAME		=	webserv

D_OBJS		=	objs/

D_SRCS		=	srcs/

_SRC_		=	main.cpp \
				autoIndex.cpp \
				cgiHandler.cpp \
				Location.cpp \
				Parser.cpp \
				Request_Parser.cpp \
				Response.cpp \
				Server.cpp \
				SockData.cpp \
				socket.cpp \
				utils.cpp \

SRCS		=	$(addprex$(D_SRCS), $(_SRC_))

OBJS		=	$(addprefix $(D_OBJS), $(_SRC_:.cpp=.o))

R			=	\033[0;31m
G			=	\033[0;32m
B			=	\033[0;34m
W			=	\033[0m

################################################################################
#####                           COMPILER OPTIONS                           #####
################################################################################

CC			=	clang++ -std=c++98

FLAGS		=	-Wall -Wextra -Werror

FSANITIZE	=	-g -fsanitize=address

################################################################################
#####                            MAKEFILE RULES                            #####
################################################################################

all	: $(D_OBJS) $(NAME)

$(D_OBJS) :
	@mkdir objs

$(D_OBJS)%.o : $(D_SRCS)%.cpp Makefile
	@$(CC) $(FLAGS) -c $< -o $@ -Iincludes
	@printf "$(B)$<$(W) linking...\n"

$(NAME) : $(OBJS) Makefile
	@printf "Compiling objects...\n"
	@$(CC) $(FSANITIZE) $(OBJS) -o $(NAME)
	@printf "[ $(G)$(NAME)$(W) ] Compiled\n"

clean :
	@rm -rf $(D_OBJS)
	@printf "[ Object files ] $(R)removed\n$(W)"

fclean : clean
	@rm -rf $(NAME)
	@printf "[ $(NAME) ] $(R)removed\n$(W)"

re : fclean all
