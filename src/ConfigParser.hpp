#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <map>

class ConfigParser {
public:
    ConfigParser(const std::string& filename);
    void parse();

    const std::map<std::string, std::string>& getConfig() const;

private:
    std::string filename;
    std::map<std::string, std::string> config;

    void parseLine(const std::string& line);
    void trim(std::string &str);
};

#endif // CONFIGPARSER_HPP
