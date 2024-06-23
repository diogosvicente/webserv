#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <map>

class ConfigParser {
public:
    ConfigParser(const std::string& filename);
    std::map<std::string, std::string> getConfig() const;

private:
    std::map<std::string, std::string> config;
    void parseConfig(const std::string& filename);
    void trim(std::string& str);
};

#endif
