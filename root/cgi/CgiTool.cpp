#include "CgiInclude.hpp"
#include <arpa/inet.h>
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::invalid_argument("Missing CGI body length");
        std::string query;
        GetQueryString(GetMethod(), query, argv[1]);
        const auto fields = KVconstructor(query);
        const auto ip = fields.find("ip"), port = fields.find("port");
        in_addr address{};
        if (ip == fields.end() || inet_pton(AF_INET, ip->second.c_str(), &address) != 1)
            throw std::invalid_argument("Provide one IPv4 address");
        if (port == fields.end() || port->second.empty() || port->second.find_first_not_of("0123456789") != std::string::npos)
            throw std::invalid_argument("Provide one numeric port");
        const unsigned long number = std::stoul(port->second);
        if (number < 1 || number > 65535) throw std::invalid_argument("Port must be between 1 and 65535");
        execl("/usr/bin/nmap", "nmap", "-p", port->second.c_str(), ip->second.c_str(), static_cast<char*>(nullptr));
        throw std::runtime_error("Nmap is unavailable at /usr/bin/nmap");
    } catch (const std::exception& error) {
        CGILOG(ERROR, error.what());
        return 1;
    }
}
