#include "../root/cgi/CgiInclude.hpp"
#include <cassert>
int main() {
    auto fields = KVconstructor("username=Ada+Lovelace&pwd=a%26b%3Dc&empty=");
    assert(fields.at("username") == "Ada Lovelace");
    assert(fields.at("pwd") == "a&b=c");
    assert(fields.at("empty").empty());
    bool rejected = false;
    try { DecodeForm("%Q1"); } catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try { DecodeForm("%"); } catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    std::cout << "CGI form checks passed\n";
}
