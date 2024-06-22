#include <iostream>
#include "Server.hpp"
#include "ConfigParser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    const std::string configFile = argv[1];

    // Parse the configuration file
    ConfigParser configParser(configFile);
    try {
        configParser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing configuration file: " << e.what() << std::endl;
        return 1;
    }

    // Create and start the server
    try {
        Server server(configParser.getConfig());
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
