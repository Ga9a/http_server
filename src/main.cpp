#include "../bin/httpserver.hpp"
#include <cstdlib>
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " port\n";
        return 2;
    }
    try {
        std::size_t end = 0;
        const int port = std::stoi(argv[1], &end);
        if (argv[1][end] != '\0' || port < 1 || port > 65535)
            throw std::invalid_argument("Port must be an integer between 1 and 65535");
        httpserver(port).activate();
    } catch (const std::exception& error) {
        std::cerr << "Server failed: " << error.what() << '\n';
        return 1;
    }
}
