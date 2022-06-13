
NAME=webserv

SRC=HTTP_request/HTTP_request.cpp main.cpp Utils.cpp config/location.cpp config/server.cpp config/HTTP_server.cpp http_response/*.cpp CGI/CGI.cpp

FLAGS=-Wall -Wextra -Werror



all: $(NAME)
	
$(NAME):
	@g++ $(FLAGS) $(SRC) -o $(NAME)

clean:
	@rm -f *.o

fclean:
	@rm -f $(NAME)

re: fclean all

debug:
	@g++ $(FLAGS) $(SRC) -o $(NAME) -fsanitize=address -g