#include "Server.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "CGIHandler.hpp"
#include "Utils.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <algorithm>
#include <sys/wait.h>


Server::Server(const std::map<std::string, std::string>& config) : config(config) {
    init();
}

void Server::init() {
    std::string portStr = config.at("listen");
    std::istringstream portStream(portStr);
    std::string port;

    while (std::getline(portStream, port, ',')) {
        int portNum = atoi(port.c_str());
        if (portNum > 0) {
            createSocket(portNum);
        } else {
            throw std::runtime_error("Invalid port number: " + port);
        }
    }
}

void Server::createSocket(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options");  // Corrigido aqui
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to bind socket on port " + intToString(port));
    }

    if (listen(listen_fd, 10) < 0) {
        throw std::runtime_error("Failed to listen on socket on port " + intToString(port));
    }

    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    pollfd pfd;
    pfd.fd = listen_fd;
    pfd.events = POLLIN;
    fds.push_back(pfd);

    listen_fds.push_back(listen_fd);

    std::cout << "Server initialized and listening on port " << port << "..." << std::endl;
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
                if (std::find(listen_fds.begin(), listen_fds.end(), fds[i].fd) != listen_fds.end()) {
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0) {
                        std::cerr << "Accept error on fd " << fds[i].fd << ": " << strerror(errno) << std::endl;
                        continue;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    pollfd pfd;
                    pfd.fd = client_fd;
                    pfd.events = POLLIN;
                    fds.push_back(pfd);

                    std::cout << "New connection accepted on port " << ntohs(client_addr.sin_port) << ", fd: " << client_fd << std::endl;
                } else {
                    int fd = fds[i].fd;
                    std::cout << "Handling request for fd: " << fd << std::endl;
                    handleRequest(fd);
                    std::cout << "Request handled for fd: " << fd << std::endl;
                    close(fd);
                    fds.erase(fds.begin() + i);
                    std::cout << "Connection closed and fd removed: " << fd << std::endl;
                    --i;
                }
            }
        }
    }
}

void Server::handleRequest(int client_fd) {
    char buffer[4096];
    int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        std::cerr << "Error reading from socket\n";
        close(client_fd);
        return;
    }
    buffer[bytes_read] = '\0';
    std::string raw_request(buffer);
    HTTPRequest request(raw_request);

    std::string method = request.getMethod();
    std::string uri = request.getPath();
    std::string requested_path = getRequestedPath(uri);

    if (!isMethodAllowed(uri, method)) {
        sendErrorResponse(client_fd, 405, "Method Not Allowed");
        return;
    }

    if (method == "POST" && uri == "/upload") {
        std::string boundary = "--" + request.getHeader("Content-Type").substr(30);
        size_t filename_pos = raw_request.find("filename=\"") + 10;
        size_t filename_end_pos = raw_request.find("\"", filename_pos);
        std::string filename = raw_request.substr(filename_pos, filename_end_pos - filename_pos);

        size_t file_content_start = raw_request.find("\r\n\r\n", filename_end_pos) + 4;
        size_t file_content_end = raw_request.find(boundary, file_content_start) - 4;

        std::string file_content = raw_request.substr(file_content_start, file_content_end - file_content_start);

        std::ofstream outfile((config.at("root") + "/upload/" + filename).c_str());
        outfile << file_content;
        outfile.close();

        HTTPResponse response;
        response.setStatusCode(200);
        response.setStatusMessage("OK");
        response.setBody("<html><body><h1>File Uploaded</h1></body></html>");
        send(client_fd, response.toString().c_str(), response.toString().length(), 0);
        close(client_fd);
        return;
    }

    if (method == "DELETE") {
        std::string decoded_path = uri;
        size_t pos = decoded_path.find('%');
        while (pos != std::string::npos) {
            if (pos + 2 < decoded_path.length()) {
                std::string hex_str = decoded_path.substr(pos + 1, 2);
                char decoded_char = static_cast<char>(strtol(hex_str.c_str(), NULL, 16));
                decoded_path.replace(pos, 3, 1, decoded_char);
            }
            pos = decoded_path.find('%', pos + 1);
        }
        requested_path = config.at("root") + decoded_path;

        if (remove(requested_path.c_str()) == 0) {
            HTTPResponse response;
            response.setStatusCode(200);
            response.setStatusMessage("OK");
            response.setBody("<html><body><h1>File Deleted</h1></body></html>");
            send(client_fd, response.toString().c_str(), response.toString().length(), 0);
        } else {
            sendErrorResponse(client_fd, 404, "Not Found");
        }
        close(client_fd);
        return;
    }

    if (method == "GET" && uri == "/cause500") {
        sendErrorResponse(client_fd, 500, "Internal Server Error");
        close(client_fd);
        return;
    }

    if (method == "GET") {
        if (uri == "/") {
            requested_path += "/index.html";
        }
        struct stat file_stat;
        if (stat(requested_path.c_str(), &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                requested_path += "/index.html";
                if (stat(requested_path.c_str(), &file_stat) != 0) {
                    sendErrorResponse(client_fd, 404, "Not Found");
                    close(client_fd);
                    return;
                }
            }
            if (requested_path.find(".php") != std::string::npos) {
                handleCGI(client_fd, requested_path, request);
            } else {
                serveFile(client_fd, requested_path);
            }
        } else {
            sendErrorResponse(client_fd, 404, "Not Found");
        }
    } else {
        sendErrorResponse(client_fd, 405, "Method Not Allowed");
    }
    close(client_fd);
}

void Server::handleCGI(int client_fd, const std::string& scriptPath, const HTTPRequest& request) {
    pid_t pid = fork();
    if (pid == 0) {
        // Processo filho
        CGIHandler cgiHandler(scriptPath, request);
        std::string cgi_output = cgiHandler.execute();
        send(client_fd, cgi_output.c_str(), cgi_output.length(), 0);
        exit(0);
    } else if (pid > 0) {
        // Processo pai
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cerr << "CGI script did not exit properly" << std::endl;
        }
    } else {
        // Falha no fork
        std::cerr << "Failed to fork for CGI execution: " << strerror(errno) << std::endl;
        sendErrorResponse(client_fd, 500, "Internal Server Error");
    }
}

std::string Server::getRequestedPath(const std::string& uri) {
    std::map<std::string, std::string>::const_iterator it = config.find("location " + uri);
    if (it != config.end()) {
        return it->second;
    }
    return config.at("root") + uri;
}

bool Server::isMethodAllowed(const std::string& uri, const std::string& method) {
    std::map<std::string, std::string>::const_iterator it = config.find("location " + uri + " allow_methods");
    if (it != config.end()) {
        std::istringstream ss(it->second);
        std::string allowed_method;
        while (ss >> allowed_method) {
            if (allowed_method == method) {
                return true;
            }
        }
        return false;
    }
    return true;
}

void Server::serveFile(int client_fd, const std::string& path) {
    std::string content = readFile(path);
    if (content.empty()) {
        sendErrorResponse(client_fd, 404, "Not Found");
        return;
    }

    HTTPResponse response;
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setHeader("Content-Type", getMimeType(path));
    response.setBody(content);

    std::string raw_response = response.toString();
    if (write(client_fd, raw_response.c_str(), raw_response.size()) < 0) {
        std::cerr << "Error writing response to fd: " << client_fd << ", error: " << strerror(errno) << std::endl;
    }
}

void Server::sendErrorResponse(int client_fd, int status_code, const std::string& status_message) {
    HTTPResponse response;
    response.setStatusCode(status_code);
    response.setStatusMessage(status_message);
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body><h1>" + intToString(status_code) + " " + status_message + "</h1></body></html>");

    std::string raw_response = response.toString();
    if (write(client_fd, raw_response.c_str(), raw_response.size()) < 0) {
        std::cerr << "Error writing error response to fd: " << client_fd << ", error: " << strerror(errno) << std::endl;
    }
}

std::string Server::getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos || path.find(".htm") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".gif") != std::string::npos) return "image/gif";
    if (path.find(".txt") != std::string::npos) return "text/plain";
    return "application/octet-stream";
}

std::string Server::readFile(const std::string& path) {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Server::intToString(int num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}
