NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = src/main.cpp \
       src/Server.cpp \
       src/ConfigParser.cpp \
       src/HTTPRequest.cpp \
       src/HTTPResponse.cpp \
       src/CGIHandler.cpp \
       src/Utils.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run test_get test_post test_delete test_responses

# Regras de execução
run:
	./webserv config/webserv.conf

test_get:
	@tests/test_get.sh

test_post:
	@tests/test_post.sh

test_delete:
	@tests/test_delete.sh

test_responses:
	@tests/test_responses.sh

test_all: test_get test_post test_delete test_responses
