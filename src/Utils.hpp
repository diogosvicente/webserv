#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <map>

class Utils {
public:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string getMimeType(const std::string& extension);
    static std::string generateErrorResponse(int statusCode);
    static std::map<int, std::string> statusCodeToReasonPhrase;
};

#endif // UTILS_HPP
