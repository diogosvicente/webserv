#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HTTPResponse {
public:
    HTTPResponse();

    void setVersion(const std::string& version);
    void setStatusCode(int code);
    void setReasonPhrase(const std::string& phrase);
    void setHeader(const std::string& field, const std::string& value);
    void setBody(const std::string& body);

    std::string toString() const;

private:
    std::string version;
    int statusCode;
    std::string reasonPhrase;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string statusCodeToReasonPhrase(int code) const;
};

#endif // HTTPRESPONSE_HPP
