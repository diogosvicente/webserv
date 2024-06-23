#include "CGIHandler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>

CGIHandler::CGIHandler(const std::string& script_path, const HTTPRequest& request)
    : script_path(script_path), request(request) {}

std::string CGIHandler::execute() {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        std::cerr << "Pipe failed: " << strerror(errno) << std::endl;
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        return "";
    } else if (pid == 0) {
        // Child process
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);

        setEnvironment();

        char* args[] = { const_cast<char*>("/usr/bin/php-cgi"), const_cast<char*>(script_path.c_str()), NULL };
        execve("/usr/bin/php-cgi", args, environ);
        std::cerr << "Failed to execute PHP CGI: " << strerror(errno) << std::endl;
        exit(1);
    } else {
        // Parent process
        close(pipe_fd[1]);
        waitpid(pid, NULL, 0);

        std::stringstream output;
        char buffer[4096];
        int bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            output.write(buffer, bytes_read);
        }
        close(pipe_fd[0]);
        return output.str();
    }
}

void CGIHandler::setEnvironment() const {
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("SCRIPT_FILENAME", script_path.c_str(), 1);
    setenv("REQUEST_METHOD", request.getMethod().c_str(), 1);
    setenv("QUERY_STRING", request.getQueryString().c_str(), 1);
    setenv("CONTENT_TYPE", request.getHeader("Content-Type").c_str(), 1);
    setenv("CONTENT_LENGTH", request.getHeader("Content-Length").c_str(), 1);
    setenv("REDIRECT_STATUS", "200", 1); // Adicionado para contornar o force-cgi-redirect
}
