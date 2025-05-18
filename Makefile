NAME= webserver
CXX= c++
CXXFLAGS= #-Wall -Wextra -Werror -std=c++98
OBJ= main.cpp WebServer.cpp network.cpp server.cpp client.cpp
OBJ_O= $(OBJ:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ_O)
	$(CXX) $(CXXFLAGS) $(OBJ_O) -o $(NAME)

clean:
	rm -rf $(OBJ_O)

fclean: clean
	rm -rf $(NAME)

re:	fclean all