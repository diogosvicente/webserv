#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest {
public:
    HTTPRequest(const std::string& raw_request);

    void parse();

    std::string getMethod() const;
    std::string getPath() const;
    std::string getVersion() const;
    std::string getHeader(const std::string& field) const;
    std::string getBody() const;

private:
    std::string raw_request;
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseRequestLine(const std::string& line);
    void parseHeaderLine(const std::string& line);
    void parseHeaders(std::istringstream& stream);
    void parseBody(std::istringstream& stream);
};

#endif // HTTPREQUEST_HPP
