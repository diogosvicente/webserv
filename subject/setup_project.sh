#!/bin/bash

# Criar estrutura de pastas
mkdir -p webserv/src
mkdir -p webserv/config
mkdir -p webserv/www/upload
mkdir -p webserv/www/cgi-bin

# Criar arquivos em src/
touch webserv/src/main.cpp
touch webserv/src/Server.cpp
touch webserv/src/Server.hpp
touch webserv/src/ConfigParser.cpp
touch webserv/src/ConfigParser.hpp
touch webserv/src/HTTPRequest.cpp
touch webserv/src/HTTPRequest.hpp
touch webserv/src/HTTPResponse.cpp
touch webserv/src/HTTPResponse.hpp
touch webserv/src/CGIHandler.cpp
touch webserv/src/CGIHandler.hpp
touch webserv/src/Utils.cpp
touch webserv/src/Utils.hpp

# Criar arquivo de configuração
cat <<EOL > webserv/config/webserv.conf
server {
    listen 8080;
    server_name localhost;

    root $(pwd)/webserv/www;
    index index.html;

    error_page 404 /404.html;

    location / {
        try_files \$uri \$uri/ =404;
    }

    location /upload {
        client_max_body_size 2M;
        autoindex on;
    }

    location /cgi-bin {
        fastcgi_pass 127.0.0.1:9000;
        include fastcgi_params;
        fastcgi_param SCRIPT_FILENAME $(pwd)/webserv/www/cgi-bin/script.php;
    }
}
EOL

# Criar arquivos em www/
echo "<!DOCTYPE html><html><head><title>Welcome</title></head><body><h1>Welcome to Webserv</h1></body></html>" > webserv/www/index.html
echo "<?php echo 'Hello, World!'; ?>" > webserv/www/cgi-bin/script.php

# Criar Makefile
cat <<EOL > webserv/Makefile
NAME = webserv

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = src/main.cpp \\
       src/Server.cpp \\
       src/ConfigParser.cpp \\
       src/HTTPRequest.cpp \\
       src/HTTPResponse.cpp \\
       src/CGIHandler.cpp \\
       src/Utils.cpp

OBJS = \$(SRCS:.cpp=.o)

all: \$(NAME)

\$(NAME): \$(OBJS)
	\$(CXX) \$(CXXFLAGS) -o \$(NAME) \$(OBJS)

clean:
	rm -f \$(OBJS)

fclean: clean
	rm -f \$(NAME)

re: fclean all

.PHONY: all clean fclean re
EOL

echo "Estrutura do projeto Webserv criada com sucesso!"
