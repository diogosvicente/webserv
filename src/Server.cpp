#include "Server.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Utils.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

Server::Server(const std::map<std::string, std::string>& config) : config(config) {
    init();
}

void Server::init() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081);  // Mudança para a nova porta

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(listen_fd, 10) < 0) {
        throw std::runtime_error("Failed to listen on socket");
    }

    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    pollfd pfd;
    pfd.fd = listen_fd;
    pfd.events = POLLIN;
    fds.push_back(pfd);

    std::cout << "Server initialized and listening on port 8081..." << std::endl;
}

void Server::run() {
    std::cout << "Server running..." << std::endl;

    while (true) {
        int poll_count = poll(&fds[0], fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            continue;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == listen_fd) {
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0) {
                        std::cerr << "Accept error: " << strerror(errno) << std::endl;
                        continue;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    pollfd pfd;
                    pfd.fd = client_fd;
                    pfd.events = POLLIN;
                    fds.push_back(pfd);

                    std::cout << "New connection accepted, fd: " << client_fd << std::endl;
                } else {
                    int fd = fds[i].fd;
                    std::cout << "Handling request for fd: " << fd << std::endl;
                    handleRequest(fd);
                    std::cout << "Request handled for fd: " << fd << std::endl;
                    close(fd);
                    fds.erase(fds.begin() + i);
                    std::cout << "Connection closed and fd removed: " << fd << std::endl;
                    --i;  // Ajuste o índice após remover um elemento
                }
            }
        }
    }
}

void Server::handleRequest(int client_fd) {
    char buffer[4096];
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        std::cerr << "Error reading from fd: " << client_fd << ", bytes_read: " << bytes_read << ", error: " << strerror(errno) << std::endl;
        return;
    }

    if (bytes_read == 0) {
        std::cerr << "Client disconnected, fd: " << client_fd << std::endl;
        return;
    }

    buffer[bytes_read] = '\0';
    std::string raw_request(buffer);
    std::cout << "Received request: " << raw_request << std::endl;

    HTTPRequest request(raw_request);
    try {
        request.parse();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing request: " << e.what() << std::endl;
        return;
    }

    HTTPResponse response;
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>Hello, World!</h1></body></html>");

    std::string raw_response = response.toString();
    if (write(client_fd, raw_response.c_str(), raw_response.size()) < 0) {
        std::cerr << "Error writing to fd: " << client_fd << ", error: " << strerror(errno) << std::endl;
    }
}
