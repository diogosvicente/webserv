#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>

class Server {
public:
    Server(const std::map<std::string, std::string>& config);
    void run();

private:
    std::vector<int> listen_fds;
    std::map<std::string, std::string> config;
    std::vector<pollfd> fds;

    void init();
    void createSocket(int port);
    void handleRequest(int client_fd);
    void serveFile(int client_fd, const std::string& path);
    std::string getMimeType(const std::string& path);
    std::string readFile(const std::string& path);
    void sendErrorResponse(int client_fd, int status_code, const std::string& status_message);
    std::string intToString(int num);
};

#endif
