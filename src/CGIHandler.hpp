#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <map>

class CGIHandler {
public:
    CGIHandler(const std::string& script_path, const std::map<std::string, std::string>& env_vars);
    
    void execute();

    std::string getOutput() const;

private:
    std::string script_path;
    std::map<std::string, std::string> env_vars;
    std::string output;

    void setEnvironment() const;
    std::string readFromPipe(int fd) const;
};

#endif // CGIHANDLER_HPP
