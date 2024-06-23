#include "CGIHandler.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

CGIHandler::CGIHandler(const std::string& script_path, const HTTPRequest& request)
    : script_path(script_path), request(request) {}

std::string CGIHandler::execute() {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        std::cerr << "Failed to create pipe: " << strerror(errno) << std::endl;
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork: " << strerror(errno) << std::endl;
        return "";
    }

    if (pid == 0) { // Child process
        close(pipe_fd[0]); // Close read end

        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        setEnvironment();

        char* args[] = { const_cast<char*>(script_path.c_str()), NULL };
        execve(script_path.c_str(), args, environ);

        std::cerr << "Failed to execute CGI script: " << strerror(errno) << std::endl;
        exit(1);
    } else { // Parent process
        close(pipe_fd[1]); // Close write end

        waitpid(pid, NULL, 0);

        std::stringstream output;
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            output.write(buffer, bytes_read);
        }
        close(pipe_fd[0]);

        return output.str();
    }
}

void CGIHandler::setEnvironment() const {
    setenv("REQUEST_METHOD", request.getMethod().c_str(), 1);
    setenv("QUERY_STRING", request.getQueryString().c_str(), 1);
    setenv("CONTENT_TYPE", request.getHeader("Content-Type").c_str(), 1);
    setenv("CONTENT_LENGTH", request.getHeader("Content-Length").c_str(), 1);
    setenv("SCRIPT_FILENAME", script_path.c_str(), 1);
    setenv("PATH_INFO", request.getPath().c_str(), 1);
}

