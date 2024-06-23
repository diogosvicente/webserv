#include "Server.hpp"
#include "ConfigParser.hpp"
#include <iostream>
#include <map>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    try {
        ConfigParser parser(argv[1]);
        std::map<std::string, std::string> config = parser.getConfig();

        Server server(config);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
