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
    server_addr.sin_port = htons(8080);

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
}

void Server::run() {
    std::cout << "Server running..." << std::endl;

    while (true) {
        int poll_count = poll(&fds[0], fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll error" << std::endl;
            continue;
        }

        for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
            if (it->revents & POLLIN) {
                if (it->fd == listen_fd) {
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0) {
                        std::cerr << "Accept error" << std::endl;
                        continue;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    pollfd pfd;
                    pfd.fd = client_fd;
                    pfd.events = POLLIN;
                    fds.push_back(pfd);
                } else {
                    handleRequest(it->fd);
                }
            }
        }
    }
}

void Server::handleRequest(int client_fd) {
    char buffer[4096];
    int bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        close(client_fd);
        for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
            if (it->fd == client_fd) {
                fds.erase(it);
                break;
            }
        }
        return;
    }

    buffer[bytes_read] = '\0';
    std::string raw_request(buffer);
    HTTPRequest request(raw_request);
    request.parse();

    HTTPResponse response;
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>Hello, World!</h1></body></html>");

    std::string raw_response = response.toString();
    write(client_fd, raw_response.c_str(), raw_response.size());
}
