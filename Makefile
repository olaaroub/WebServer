CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
# CXXFLAGS += -fsanitize=address -g3
# MAKEFLAGS = -j

NAME = webserv

SPATH = ./Sources/
SRC = 	main.cpp \
 		ServerManager.cpp\
		CgiExecutor.cpp \
		HttpResponse.cpp \
		client.cpp \
		network.cpp \
		server.cpp \
		Headers.cpp \
		request.cpp \
		RequestLine.cpp \
		Utils.cpp \
		ConfigParse/ConfigFileParser.cpp \
		ConfigParse/ConfigFileReader.cpp\
		ConfigParse/Configs.cpp\
		Methods/Get.cpp \
		Methods/Post.cpp \
		Methods/Delete.cpp \



SRCS = $(addprefix $(SPATH), $(SRC))

OPATH = ./dependencies/
OBJ = $(SRC:.cpp=.o)
OBJS = $(addprefix $(OPATH),$(OBJ))
DEPS = $(OBJS:.o=.d)

INC			=	-I ./Includes/\

all: $(OPATH)  $(NAME)

$(OPATH):
	mkdir -p $(OPATH)
	mkdir -p $(OPATH)ConfigParse
	mkdir -p $(OPATH)Methods


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

re: fclean
	make all

.PHONY: all clean fclean re

