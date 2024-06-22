#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Constructor: initializes the parser with the config file path
ConfigParser::ConfigParser(const std::string& filename) : filename(filename) {}

// Public method to parse the configuration file
void ConfigParser::parse() {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open config file.");
    }
    
    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
}

// Parses a single line of the configuration file
void ConfigParser::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;
    if (std::getline(iss, key, ' ') && std::getline(iss, value)) {
        trim(key);
        trim(value);
        if (!key.empty() && !value.empty()) {
            config[key] = value;
        }
    }
}

// Trims leading and trailing whitespace from a string
void ConfigParser::trim(std::string &str) {
    // Trim leading whitespace
    std::string::iterator it = str.begin();
    while (it != str.end() && std::isspace(*it)) {
        ++it;
    }
    str.erase(str.begin(), it);

    // Trim trailing whitespace
    if (!str.empty()) {
        it = str.end() - 1;
        while (it != str.begin() && std::isspace(*it)) {
            --it;
        }
        if (!std::isspace(*it)) {
            ++it;
        }
        str.erase(it, str.end());
    }
}

// Returns the parsed configuration as a map
const std::map<std::string, std::string>& ConfigParser::getConfig() const {
    return config;
}
