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
    std::map<std::string, std::string> config;
    std::vector<pollfd> fds;
    int listen_fd;

    void init();
    void handleConnections();
    void handleRequest(int client_fd);
};

#endif // SERVER_HPP
