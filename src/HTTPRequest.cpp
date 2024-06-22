#include "HTTPRequest.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

// Constructor: initializes the request with the raw request string
HTTPRequest::HTTPRequest(const std::string& raw_request) : raw_request(raw_request) {}

// Public method to parse the raw HTTP request
void HTTPRequest::parse() {
    std::istringstream stream(raw_request);
    std::string line;

    // Parse the request line
    if (std::getline(stream, line)) {
        parseRequestLine(line);
    }

    // Parse headers
    parseHeaders(stream);

    // Parse body
    parseBody(stream);
}

// Returns the HTTP method (GET, POST, DELETE, etc.)
std::string HTTPRequest::getMethod() const {
    return method;
}

// Returns the requested path
std::string HTTPRequest::getPath() const {
    return path;
}

// Returns the HTTP version
std::string HTTPRequest::getVersion() const {
    return version;
}

// Returns the value of a specific header field
std::string HTTPRequest::getHeader(const std::string& field) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(field);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

// Returns the body of the request
std::string HTTPRequest::getBody() const {
    return body;
}

// Private method to parse the request line (e.g., "GET /path HTTP/1.1")
void HTTPRequest::parseRequestLine(const std::string& line) {
    std::istringstream stream(line);
    stream >> method >> path >> version;
}

// Private method to parse a single header line
void HTTPRequest::parseHeaderLine(const std::string& line) {
    std::string::size_type pos = line.find(":");
    if (pos != std::string::npos) {
        std::string field = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace from field and value
        field.erase(field.begin(), std::find_if(field.begin(), field.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        field.erase(std::find_if(field.rbegin(), field.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), field.end());
        value.erase(value.begin(), std::find_if(value.begin(), value.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        value.erase(std::find_if(value.rbegin(), value.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), value.end());

        headers[field] = value;
    }
}

// Private method to parse all headers
void HTTPRequest::parseHeaders(std::istringstream& stream) {
    std::string line;
    while (std::getline(stream, line) && line != "\r") {
        parseHeaderLine(line);
    }
}

// Private method to parse the body of the request
void HTTPRequest::parseBody(std::istringstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        body += line + "\n";
    }
}
