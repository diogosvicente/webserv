#include "HTTPResponse.hpp"
#include <sstream>

HTTPResponse::HTTPResponse() : version("HTTP/1.1"), statusCode(200), reasonPhrase("OK") {}

void HTTPResponse::setVersion(const std::string& version) {
    this->version = version;
}

void HTTPResponse::setStatusCode(int code) {
    statusCode = code;
    reasonPhrase = statusCodeToReasonPhrase(code);
}

void HTTPResponse::setReasonPhrase(const std::string& phrase) {
    reasonPhrase = phrase;
}

void HTTPResponse::setHeader(const std::string& field, const std::string& value) {
    headers[field] = value;
}

void HTTPResponse::setBody(const std::string& body) {
    this->body = body;
}

std::string HTTPResponse::toString() const {
    std::ostringstream response;

    // Status line
    response << version << " " << statusCode << " " << reasonPhrase << "\r\n";

    // Headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }

    // Blank line to separate headers from body
    response << "\r\n";

    // Body
    response << body;

    return response.str();
}

std::string HTTPResponse::statusCodeToReasonPhrase(int code) const {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}
