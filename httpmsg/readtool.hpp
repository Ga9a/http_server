#pragma once
#include "../bin/include.hpp"
class ReadTool {
public:
    static bool CutString(const std::string& value, std::string& left, std::string& right, const std::string& separator) {
        const auto position = value.find(separator);
        if (position == std::string::npos) return false;
        left = value.substr(0, position);
        right = value.substr(position + separator.size());
        return true;
    }
    // Accept CRLF or LF lines, capped at 8 KiB to bound header memory.
    static int ReadLine(int socket, std::string& output) {
        output.clear();
        char byte;
        while (output.size() < 8192) {
            const ssize_t count = recv(socket, &byte, 1, 0);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) return static_cast<int>(count);
            if (byte == '\n') {
                if (!output.empty() && output.back() == '\r') output.pop_back();
                output.push_back('\n');
                return static_cast<int>(output.size());
            }
            output.push_back(byte);
        }
        return -1;
    }
};
