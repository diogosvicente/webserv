#include "Utils.hpp"
#include <algorithm>
#include <sstream>

// Trim leading and trailing whitespace from a string
std::string Utils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Split a string by a given delimiter
std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Get the MIME type based on the file extension
std::string Utils::getMimeType(const std::string& extension) {
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    if (extension == ".txt") return "text/plain";
    // Add more MIME types as needed
    return "application/octet-stream";
}

// Generate a simple HTML error response based on the status code
std::string Utils::generateErrorResponse(int statusCode) {
    std::string reasonPhrase = statusCodeToReasonPhrase[statusCode];
    std::ostringstream oss;
    oss << "<html><head><title>" << statusCode << " " << reasonPhrase << "</title></head>"
        << "<body><h1>" << statusCode << " " << reasonPhrase << "</h1></body></html>";
    return oss.str();
}

// Initialize the static member
std::map<int, std::string> Utils::statusCodeToReasonPhrase;

struct StatusCodeToReasonPhraseInitializer {
    StatusCodeToReasonPhraseInitializer() {
        Utils::statusCodeToReasonPhrase[200] = "OK";
        Utils::statusCodeToReasonPhrase[201] = "Created";
        Utils::statusCodeToReasonPhrase[204] = "No Content";
        Utils::statusCodeToReasonPhrase[400] = "Bad Request";
        Utils::statusCodeToReasonPhrase[403] = "Forbidden";
        Utils::statusCodeToReasonPhrase[404] = "Not Found";
        Utils::statusCodeToReasonPhrase[500] = "Internal Server Error";
        Utils::statusCodeToReasonPhrase[501] = "Not Implemented";
        Utils::statusCodeToReasonPhrase[502] = "Bad Gateway";
        Utils::statusCodeToReasonPhrase[503] = "Service Unavailable";
    }
};

// Instantiate the initializer to populate the map
static StatusCodeToReasonPhraseInitializer statusCodeToReasonPhraseInitializer;
