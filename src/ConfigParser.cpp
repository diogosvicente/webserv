#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

ConfigParser::ConfigParser(const std::string& filename) {
    parseConfig(filename);
}

std::map<std::string, std::string> ConfigParser::getConfig() const {
    return config;
}

void ConfigParser::parseConfig(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file");
    }

    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '{' || line[0] == '}') {
            continue;
        }

        size_t delimiter_pos = line.find(' ');
        if (delimiter_pos == std::string::npos) {
            throw std::runtime_error("Invalid config line: " + line);
        }

        std::string key = line.substr(0, delimiter_pos);
        std::string value = line.substr(delimiter_pos + 1);

        trim(key);
        trim(value);

        // Remove trailing semicolon if present
        if (!value.empty() && value[value.size() - 1] == ';') {
            value.erase(value.size() - 1);
        }

        trim(value);
        config[key] = value;
    }
}

void ConfigParser::trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
}
