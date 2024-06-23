#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include "HTTPRequest.hpp"

class CGIHandler {
public:
    CGIHandler(const std::string& script_path, const HTTPRequest& request);
    std::string execute();

private:
    std::string script_path;
    HTTPRequest request;
    void setEnvironment() const;
};

#endif
