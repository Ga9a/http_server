#include "CgiInclude.hpp"
#include <mysql.h>
const char* Environment(const char* name, const char* fallback) {
    const char* value = std::getenv(name);
    return value && *value ? value : fallback;
}
bool UserCheck(const std::unordered_map<std::string, std::string>& fields) {
    const auto username = fields.find("username"), password = fields.find("pwd");
    if (username == fields.end() || password == fields.end() || username->second.empty() ||
        username->second.size() > 50 || password->second.empty() || password->second.size() > 50) return false;
    const char* database_password = std::getenv("MYSQL_PASSWORD");
    if (!database_password) return false;
    MYSQL* connection = mysql_init(nullptr);
    if (!connection) return false;
    unsigned int timeout = 3;
    mysql_options(connection, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    if (!mysql_real_connect(connection, Environment("MYSQL_HOST", "127.0.0.1"),
        Environment("MYSQL_USER", "webserver"), database_password,
        Environment("MYSQL_DATABASE", "webserver"), 3306, nullptr, 0)) {
        mysql_close(connection);
        return false;
    }
    std::string escaped(username->second.size() * 2 + 1, '\0');
    const auto length = mysql_real_escape_string(connection, &escaped[0], username->second.data(), username->second.size());
    escaped.resize(length);
    const std::string query = "SELECT passwd FROM user WHERE username = '" + escaped + "' LIMIT 1";
    bool authenticated = false;
    if (mysql_query(connection, query.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(connection);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row && row[0]) authenticated = password->second == row[0];
            mysql_free_result(result);
        }
    }
    mysql_close(connection);
    return authenticated;
}
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::invalid_argument("Missing CGI body length");
        std::string query;
        GetQueryString(GetMethod(), query, argv[1]);
        const std::string page = std::string(WEB_ROOT_PATH) +
            (UserCheck(KVconstructor(query)) ? "/penetration.html" : "/mysqlerror.html");
        std::ifstream file(page, std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open the login response page");
        std::cout << file.rdbuf();
        return 0;
    } catch (const std::exception& error) {
        CGILOG(ERROR, error.what());
        return 1;
    }
}
