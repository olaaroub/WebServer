NAME= webserver
CXX= c++
CXXFLAGS= #-Wall -Wextra -Werror -std=c++98
HTTPCONNECTION= network.cpp server.cpp client.cpp
REQUEST= Body.cpp RequestLine.cpp Headers.cpp request.cpp
METHODS= Methods.cpp  Get.cpp  Post.cpp  Delete.cpp 
OBJ= main.cpp WebServer.cpp $(addprefix httpconnection/, $(HTTPCONNECTION)) $(addprefix request/, $(REQUEST)) $(addprefix methods/, $(METHODS))
OBJ_O= $(OBJ:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ_O)
	$(CXX) $(CXXFLAGS) $(OBJ_O) -o $(NAME)

clean:
	rm -rf $(OBJ_O)

fclean: clean
	rm -rf $(NAME)

re:	fclean all