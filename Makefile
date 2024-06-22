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

.PHONY: all clean fclean re run run1 run2 run3 test_all

# Regras de execução
run:
	./webserv config/webserv.conf

run1:
	@echo "Testing 200 OK response..."; \
	curl -i "http://localhost:8081"; \
	echo "Testing 404 Not Found response..."; \
	curl -i "http://localhost:8081/nonexistent";

run2:
	@echo "Testing HTML file..."; \
	curl -i "http://localhost:8081/index.html"; \
	echo "Testing CSS file..."; \
	curl -i "http://localhost:8081/styles.css";

run3:
	@echo "Testing PHP script..."; \
	curl -i "http://localhost:8081/cgi-bin/test.php";

test_all: run1 run2 run3
