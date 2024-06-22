#include "CGIHandler.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstdlib>  // for exit()
extern char **environ; // for environment variables

CGIHandler::CGIHandler(const std::string& script_path, const std::map<std::string, std::string>& env_vars)
    : script_path(script_path), env_vars(env_vars) {}

void CGIHandler::execute() {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork");
    } else if (pid == 0) {
        // Child process
        close(pipe_fd[0]); // Close read end
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe
        close(pipe_fd[1]);

        setEnvironment();

        execl(script_path.c_str(), script_path.c_str(), (char*)NULL);
        // If execl fails
        std::cerr << "Failed to execute CGI script: " << strerror(errno) << std::endl;
        exit(1);
    } else {
        // Parent process
        close(pipe_fd[1]); // Close write end
        output = readFromPipe(pipe_fd[0]);
        close(pipe_fd[0]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            throw std::runtime_error("CGI script exited with an error");
        }
    }
}

std::string CGIHandler::getOutput() const {
    return output;
}

void CGIHandler::setEnvironment() const {
    for (std::map<std::string, std::string>::const_iterator it = env_vars.begin(); it != env_vars.end(); ++it) {
        setenv(it->first.c_str(), it->second.c_str(), 1);
    }
}

std::string CGIHandler::readFromPipe(int fd) const {
    std::ostringstream oss;
    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        oss.write(buffer, bytes_read);
    }

    return oss.str();
}
