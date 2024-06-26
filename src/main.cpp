#include "ConfigParser.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    ConfigParser parser(config_file);
    std::vector<std::map<std::string, std::string> > server_configs = parser.getConfig();

    std::vector<Server> servers;
    for (std::vector<std::map<std::string, std::string> >::const_iterator it = server_configs.begin(); it != server_configs.end(); ++it) {
        try {
            servers.push_back(Server(*it));
        } catch (const std::exception& e) {
            std::cerr << "Server initialization error: " << e.what() << std::endl;
            return 1;
        }
    }

    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        it->run();
    }

    return 0;
}
