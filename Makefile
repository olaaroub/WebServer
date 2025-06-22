CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
# CXXFLAGS += -fsanitize=address -g3

NAME = webserv

SPATH = ./Sources/
SRC = 	main.cpp \
 		WebServer.cpp\
		test.cpp \
		ConfigParse/ConfigParser.cpp \
		Connections/client.cpp \
		Connections/network.cpp \
		Connections/server.cpp \
		Methods/Delete.cpp \
		Methods/Get.cpp \
		Methods/Methods.cpp \
		Methods/Post.cpp \
		Requests/Body.cpp \
		Requests/Headers.cpp \
		Requests/request.cpp \
		Requests/RequestLine.cpp \

SRCS = $(addprefix $(SPATH), $(SRC))

OPATH = ./dependencies/
OBJ = $(SRC:.cpp=.o)
OBJS = $(addprefix $(OPATH),$(OBJ))
DEPS = $(OBJS:.o=.d)

INC			=	-I ./Includes/\

all: $(OPATH)  $(NAME)

$(OPATH):
	mkdir -p $(OPATH)
	mkdir -p $(OPATH)/ConfigParser
	mkdir -p $(OPATH)/Connections
	mkdir -p $(OPATH)/Methods
	mkdir -p $(OPATH)/Requests


$(OPATH)%.o:  $(SPATH)%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INC)


$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(INC)


-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)
	rm -rf $(OPATH)

re: fclean all

.PHONY: all clean fclean re

