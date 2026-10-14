#pragma once
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unistd.h>
using std::cout;
using std::cerr;
using std::endl;
#define CGILOG(level, message) (std::cerr << "CGI " << #level << ": " << message << '\n')
inline char* GetMethod() {
    char* method = std::getenv("METHOD");
    if (!method) throw std::invalid_argument("Missing CGI method");
    return method;
}
inline void GetQueryString(const std::string& method, std::string& query, const std::string& length) {
    if (method == "GET") {
        const char* value = std::getenv("QUERY_STRING");
        query = value ? value : "";
    } else if (method == "POST") {
        if (length.empty() || length.find_first_not_of("0123456789") != std::string::npos)
            throw std::invalid_argument("Invalid CGI content length");
        std::size_t remaining = std::stoul(length);
        if (remaining > 1024 * 1024) throw std::invalid_argument("CGI body is too large");
        char buffer[4096];
        while (remaining) {
            const ssize_t count = read(STDIN_FILENO, buffer, std::min(remaining, sizeof(buffer)));
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) throw std::runtime_error("Incomplete CGI body");
            query.append(buffer, static_cast<std::size_t>(count));
            remaining -= static_cast<std::size_t>(count);
        }
    } else throw std::invalid_argument("Unsupported CGI method");
}
inline std::string DecodeForm(const std::string& value) {
    auto hex = [](char ch) -> int {
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
        if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
        return -1;
    };
    std::string result;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '+') result += ' ';
        else if (value[i] == '%') {
            if (i + 2 >= value.size() || hex(value[i + 1]) < 0 || hex(value[i + 2]) < 0)
                throw std::invalid_argument("Invalid form escape");
            result += static_cast<char>(hex(value[i + 1]) * 16 + hex(value[i + 2]));
            i += 2;
        } else result += value[i];
    }
    return result;
}
inline std::unordered_map<std::string, std::string> KVconstructor(const std::string& query) {
    std::unordered_map<std::string, std::string> fields;
    std::size_t start = 0;
    while (start < query.size()) {
        const std::size_t end = query.find('&', start);
        const std::string pair = query.substr(start, end == std::string::npos ? end : end - start);
        const std::size_t equals = pair.find('=');
        if (equals != std::string::npos) fields[DecodeForm(pair.substr(0, equals))] = DecodeForm(pair.substr(equals + 1));
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return fields;
}
