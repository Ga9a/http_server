
#pragma once

#include "../bin/include.hpp"
#include "readtool.hpp"
#include <spawn.h>
#include <poll.h>
extern char** environ;

#define CHECK_STOP(stop, end) \
    do { \
        if (stop) { \
            end = false; \
            return end; \
        } \
    } while (0)
#define END_BUILD do{_BulidResponse(); return; }while(0)

using namespace std;

enum StatusCode
{
  OK = 200,
  BAD_REQUEST = 400,
  NOT_FOUND = 404,
  SERVER_ERROR = 500
};

const static char *WEB_ROOT = WEB_ROOT_PATH;
const static char *HOME_PAGE = "index.html";
const static char *END_OF_LINE = "\r\n";

const static char *BAD_REQUEST_PAGE = "400.html";
const static char *NOT_FOUND_PAGE = "404.html";
const static char *SERVER_ERROR_PAGE = "500.html";

static string StatusCodeToDesc(size_t statusCode)
{

    static std::unordered_map<std::size_t, std::string> statusDescMap =
  {
    {200, "OK"},
    {400, "BAD_REQUEST"},
    {404, "NOT_FOUND"},
    {500, "SERVER_ERROR"},
  };

  if(statusDescMap.count(statusCode))
    return statusDescMap[statusCode];
  return "";
}

static string SuffixToDesc(const string &suffix)
{
  static std::unordered_map<std::string, std::string> suffixMap =
  {
    {"html", "text/html"},
    {"css", "text/css"},
    {"js", "application/javascript"},
    {"jpg", "application/x-jpg"},
    {"xml", "application/xml"}
  };

  return suffixMap[suffix];
}

class HttpRequest
{
public:
  std::string _requestLine;
  bool _cgi;
  std::string _method;
  std::string _uri;
  std::string _path;
  size_t _size;
  std::string _suffix;
  std::string _queryString;
  std::string _version;
  std::vector<std::string> _requestHeader;
  std::unordered_map<std::string, std::string> _headKV;
  size_t _contentLenth;
  std::string _blank;
  std::string _requestBody;

  HttpRequest()
    : _cgi(false)
    , _size(0)
    , _contentLenth(0)
  {}
};
class HttpResponse{
public:
  std::string _statusLine;
  std::vector<std::string> _responseHeader;
  std::string _blank;
  std::string _responseBody;

  size_t _statusCode;
  int _fd;
public:
  HttpResponse()
    : _blank(END_OF_LINE)
    , _statusCode(OK)
    , _fd(-1)
  {}

  ~HttpResponse()
  {
    if(_fd >= 0) close(_fd);
  }
};

class ServerPoint
{
    private:
    int _sock;
    HttpRequest _httpRequest;
    HttpResponse _httpResponse;
    bool stop;

    public:

    ServerPoint(int sock): _sock(sock),stop(false){};

    ~ServerPoint(){};

    private:

    // Read the request line.
    void RecvRequestLine()
    {
        assert(_sock>=0);
        if(ReadTool::ReadLine(_sock,_httpRequest._requestLine)>0)
        {
            _httpRequest._requestLine.pop_back();
            LOG( "recv request line : " + _httpRequest._requestLine,INFO);
        }
        else
        {

            stop = true;
            LOG("RecvRequestLine failed",ERROR);
            return;
        }

    }

    // Read at most 100 bounded header lines.
    void RecvRequestHeader()
    {

        vector<string> &header = _httpRequest._requestHeader;
        string tmp;
        int tmp_len;
        while(1)
        {
            if (header.size() >= 100) { stop = true; return; }
            tmp_len = ReadTool::ReadLine(_sock,tmp);
            if(tmp_len<=0)
            {
                stop = true;
                LOG("RecvRequestHeader failed",ERROR);
                return;
            }
            if(tmp_len == 1)
            {
            _httpRequest._blank = tmp;
             break;
             }

            tmp.pop_back();
            header.push_back(tmp);
            tmp.clear();
        }
    }

    // Read the declared request body without logging credentials.
    void RecvRequestBody()
    {
        if(IsNeedToRecvRequestBody())
        {
        string& body = _httpRequest._requestBody;
        char ch = 0;
        int contentLength = _httpRequest._contentLenth;
        while(contentLength--)
        {
           ssize_t s = recv(_sock, &ch, 1, 0);
           if(s > 0)
           {
              body.push_back(ch);
           }
           else
           {
             LOG("RecvRequestBody failed",ERROR);
             stop = true;
             return;
           }
          }

     }
     }

    // Validate the request line and protocol version.
    void ResolveRequestLine()
    {
        stringstream s(_httpRequest._requestLine);
        if (!(s >> _httpRequest._method >> _httpRequest._uri >> _httpRequest._version) ||
            (_httpRequest._version != "HTTP/1.0" && _httpRequest._version != "HTTP/1.1")) {
            _httpRequest._version = "HTTP/1.1";
            _httpResponse._statusCode = BAD_REQUEST;
        }

        transform(_httpRequest._method.begin(), _httpRequest._method.end(), _httpRequest._method.begin(), ::toupper);

        // cout << "------------------------------" << endl;
        // cout << _httpRequest._requestLine << endl;
        // cout << _httpRequest._method << "==" << _httpRequest._uri << "==" << _httpRequest._version << endl;
        // cout << "------------------------------" << endl;
    }

    // Index request headers.
    void ResolveRequestHeader()
    {
        string key,value;
        for(auto &str: _httpRequest._requestHeader)
        {
            if(ReadTool::CutString(str, key, value, ": "))
            {
                _httpRequest._headKV.insert({key, value});
            }
             else
            {
                LOG("_httpRequest._requestHeader CutString failed",ERROR);
            }
        }
    }

    // Validate a POST Content-Length and cap the body at 1 MiB.
    bool IsNeedToRecvRequestBody()
    {
    if(_httpRequest._method == "POST")
    {
      const auto length = _httpRequest._headKV.find("Content-Length");
      if (length == _httpRequest._headKV.end() || length->second.empty() ||
          length->second.find_first_not_of("0123456789") != std::string::npos) {
          _httpResponse._statusCode = BAD_REQUEST;
          return false;
      }
      try { _httpRequest._contentLenth = std::stoul(length->second); }
      catch (const std::exception&) { _httpResponse._statusCode = BAD_REQUEST; return false; }
      if (_httpRequest._contentLenth > 1024 * 1024) {
          _httpResponse._statusCode = BAD_REQUEST;
          return false;
      }
      if(_httpRequest._contentLenth != 0)
      {
        LOG(std::move("POST method, recv Content-Length: " + _httpRequest._headKV["Content-Length"]),INFO);
        return true;
      }

      LOG("POST method but recv Content-Lenth is 0",WARNING);
    }

    return false;
  }

    private:

    // Accept GET and POST and separate query parameters.
    bool CheckMethod()
    {
        auto &method = _httpRequest._method;
    if(method != "GET" && method != "POST")
    {
      LOG( "client method is :" + method,WARNING);
      _httpResponse._statusCode = BAD_REQUEST;
      return false;
    }
    else if(method == "GET")
    {
        size_t pos = _httpRequest._uri.find('?');
        if(pos != string::npos)
      {
        if(ReadTool::CutString(_httpRequest._uri, _httpRequest._path, _httpRequest._queryString, "?"))
        {
          _httpRequest._cgi = true;
        }
        else
        {
          LOG("uri have arguments, but split path:arguments failed",ERROR);
        }
      }
        else
      {
        _httpRequest._path = _httpRequest._uri;
      }
    }
    else
    {
          _httpRequest._cgi = true;
          _httpRequest._path = _httpRequest._uri;
    }
    return true;
    }

    // Resolve files and identify supported CGI executables.
    bool CheckFile()
    {
        struct stat fileStat;

        if(stat(_httpRequest._path.c_str(), &fileStat) == -1)
        {
            LOG( "path " + _httpRequest._path + " NOT_FOUND, err:" + string(strerror(errno)),ERROR);
            _httpResponse._statusCode = NOT_FOUND;
            return false;
        }
        else
    {

      if(S_ISDIR(fileStat.st_mode))
      {
        _httpRequest._path += '/';
        _httpRequest._path += HOME_PAGE;
        stat(_httpRequest._path.c_str(), &fileStat);
      }

      else if((fileStat.st_mode & S_IXUSR) || (fileStat.st_mode & S_IXGRP) || (fileStat.st_mode & S_IXOTH))
      {
        _httpRequest._cgi = true;
      }
      _httpRequest._size = fileStat.st_size;
      return true;
    }
    }

    // Prefix the build document root and resolve index pages.
    void SetIndex()
    {

    _httpRequest._path = WEB_ROOT + _httpRequest._path;
    if(_httpRequest._path[_httpRequest._path.size() - 1] == '/')
    {

      _httpRequest._path += HOME_PAGE;
    }
    }

    // Determine the response MIME type.
    void SetSuffix()
    {
      size_t findSuffix = _httpRequest._path.rfind('.');
    if(findSuffix == std::string::npos)
    {
      _httpRequest._suffix = "html";
    }
    else
    {
      _httpRequest._suffix = _httpRequest._path.substr(findSuffix + 1);
    }
    }

    // Construct success or error responses.
    void _BulidResponse()
  {
    switch(_httpResponse._statusCode)
    {
    case OK:
      HandlerOK();
      break;
    case NOT_FOUND:
      HandlerERROR(NOT_FOUND_PAGE);
      break;
    case BAD_REQUEST:
      HandlerERROR(BAD_REQUEST_PAGE);
      break;
    case SERVER_ERROR:
      HandlerERROR(SERVER_ERROR_PAGE);
      break;
    default:
      HandlerERROR(NOT_FOUND_PAGE);
      break;
    }
  };

    // Open a static response file.
    size_t ProcessNonCgi()
  {

    _httpResponse._fd = open(_httpRequest._path.c_str(), O_RDONLY);
    if(_httpResponse._fd >= 0)
    {
      return OK;
    }

    return NOT_FOUND;
  }
    // Spawn an allowlisted CGI program with private pipes and bounded output.
    size_t ProcessCgi() {
        int input[2], output[2];
        if (pipe2(input, O_CLOEXEC) != 0) return SERVER_ERROR;
        if (pipe2(output, O_CLOEXEC) != 0) {
            close(input[0]); close(input[1]);
            return SERVER_ERROR;
        }
        std::vector<std::string> environment;
        for (char** item = environ; *item; ++item) {
            const std::string value(*item);
            if (value.compare(0, 7, "METHOD=") && value.compare(0, 13, "QUERY_STRING="))
                environment.push_back(value);
        }
        environment.push_back("METHOD=" + _httpRequest._method);
        environment.push_back("QUERY_STRING=" + _httpRequest._queryString);
        std::vector<char*> env;
        for (auto& value : environment) env.push_back(&value[0]);
        env.push_back(nullptr);
        std::string length = std::to_string(_httpRequest._contentLenth);
        char* arguments[] = {&_httpRequest._path[0], &length[0], nullptr};
        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);
        posix_spawn_file_actions_adddup2(&actions, output[0], STDIN_FILENO);
        posix_spawn_file_actions_adddup2(&actions, input[1], STDOUT_FILENO);
        posix_spawn_file_actions_addclose(&actions, input[0]);
        posix_spawn_file_actions_addclose(&actions, output[1]);
        pid_t pid;
        const int result = posix_spawn(&pid, _httpRequest._path.c_str(), &actions, nullptr, arguments, env.data());
        posix_spawn_file_actions_destroy(&actions);
        close(input[1]); close(output[0]);
        if (result != 0) {
            close(input[0]); close(output[1]);
            return SERVER_ERROR;
        }
        bool ok = true;
        std::size_t sent = 0;
        while (sent < _httpRequest._requestBody.size()) {
            const ssize_t count = write(output[1], _httpRequest._requestBody.data() + sent, _httpRequest._requestBody.size() - sent);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) { ok = false; break; }
            sent += static_cast<std::size_t>(count);
        }
        close(output[1]);
        char buffer[4096];
        while (ok) {
            pollfd event{input[0], POLLIN, 0};
            int ready;
            do { ready = poll(&event, 1, 10000); } while (ready < 0 && errno == EINTR);
            if (ready <= 0) { ok = false; break; }
            const ssize_t count = read(input[0], buffer, sizeof(buffer));
            if (count < 0 && errno == EINTR) continue;
            if (count < 0) { ok = false; break; }
            if (count == 0) break;
            _httpResponse._responseBody.append(buffer, static_cast<std::size_t>(count));
            if (_httpResponse._responseBody.size() > 1024 * 1024) { ok = false; break; }
        }
        close(input[0]);
        if (!ok) kill(pid, SIGKILL);
        int status = 0;
        pid_t waited;
        do { waited = waitpid(pid, &status, 0); } while (waited < 0 && errno == EINTR);
        return ok && waited == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0 ? OK : SERVER_ERROR;
    }

    // Generate status and length headers.
    void HandlerOK()
  {

    _httpResponse._statusLine += _httpRequest._version;
    _httpResponse._statusLine += ' ';
    _httpResponse._statusLine += std::to_string(_httpResponse._statusCode);
    _httpResponse._statusLine += ' ';
    _httpResponse._statusLine += StatusCodeToDesc(_httpResponse._statusCode);
    _httpResponse._statusLine += END_OF_LINE;

    cout << "_httpResponse._statusLine.c_str() == " << _httpResponse._statusLine << endl;

    std::string tmp = "Content-Length: ";
    if(_httpRequest._cgi == true)
    {
      tmp += std::to_string(_httpResponse._responseBody.size());
    }
    else
    {
      tmp += std::to_string(_httpRequest._size);
    }
    tmp += END_OF_LINE;
    _httpResponse._responseHeader.push_back(tmp);

    tmp = "Content-Type: ";
    tmp += _httpRequest._path.find("/CgiTool") != std::string::npos ? "text/plain; charset=utf-8" : SuffixToDesc(_httpRequest._suffix);
    tmp += END_OF_LINE;
    _httpResponse._responseHeader.push_back(tmp);

  }
    // Generate an error page response.
    void HandlerERROR(const std::string& page)
  {
    _httpRequest._cgi = false;
    std::string pagePath = WEB_ROOT;
    pagePath += '/';
    pagePath += page;
    _httpResponse._fd = open(pagePath.c_str(), O_RDONLY);
    if(_httpResponse._fd >= 0)
    {

      _httpResponse._statusLine += _httpRequest._version;
      _httpResponse._statusLine += ' ';
      _httpResponse._statusLine += std::to_string(_httpResponse._statusCode);
      _httpResponse._statusLine += ' ';
      _httpResponse._statusLine += StatusCodeToDesc(_httpResponse._statusCode);
      _httpResponse._statusLine += END_OF_LINE;

      std::string tmp = "Content-Length: ";

      struct stat st;
      stat(pagePath.c_str(), &st);
      tmp += std::to_string(st.st_size);
      tmp += END_OF_LINE;
      _httpResponse._responseHeader.push_back(tmp);

      tmp = "Content-Type: ";
      tmp += SuffixToDesc(_httpRequest._suffix);
      tmp += END_OF_LINE;
      _httpResponse._responseHeader.push_back(tmp);

      _httpRequest._size = st.st_size;
    }
  }

    public:

    // Receive and validate one request per connection.
    bool RecvRequest()
    {
        bool end_state = true;
        RecvRequestLine();
        CHECK_STOP(stop,end_state);
        RecvRequestHeader();
        CHECK_STOP(stop,end_state);
        ResolveRequestLine();
        ResolveRequestHeader();
        RecvRequestBody();
        if(stop == false)
        {
          LOG( "recv client's request success",INFO);
        }
        else
        {
          LOG("recv requestBody failed",WARNING);
          end_state = false;
        }
        return end_state;
    }

    // Reject unsafe paths before selecting static or CGI response handling.
    void BuildResponse()
    {

    if (_httpResponse._statusCode != OK) END_BUILD;
    if(!CheckMethod()) END_BUILD;
    if (_httpRequest._path.empty() || _httpRequest._path.front() != '/' ||
        _httpRequest._path.find("..") != string::npos || _httpRequest._path.find('%') != string::npos ||
        _httpRequest._path.find('\\') != string::npos) {
        _httpResponse._statusCode = BAD_REQUEST;
        END_BUILD;
    }
    SetIndex();
    if(!CheckFile()) END_BUILD;
    SetSuffix();
    if (_httpRequest._cgi) {
        const string filename = _httpRequest._path.substr(_httpRequest._path.find_last_of('/') + 1);
        if (filename != "CgiTool" && filename != "CgiMySQL") {
            _httpResponse._statusCode = BAD_REQUEST;
        } else _httpResponse._statusCode = ProcessCgi();
    } else _httpResponse._statusCode = ProcessNonCgi();
    END_BUILD;
};
    // Send bytes until complete or the connection fails.
    bool SendAll(const char* data, std::size_t size) {
        std::size_t offset = 0;
        while (offset < size) {
            const ssize_t count = send(_sock, data + offset, size - offset, 0);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) return false;
            offset += static_cast<std::size_t>(count);
        }
        return true;
    }
    // Transmit the complete response, including partial file writes.
    void SendResponse() {
        if (!SendAll(_httpResponse._statusLine.data(), _httpResponse._statusLine.size())) return;
        for (const auto& header : _httpResponse._responseHeader)
            if (!SendAll(header.data(), header.size())) return;
        if (!SendAll(_httpResponse._blank.data(), _httpResponse._blank.size())) return;
        if (_httpRequest._cgi) {
            SendAll(_httpResponse._responseBody.data(), _httpResponse._responseBody.size());
        } else {
            off_t offset = 0;
            while (static_cast<std::size_t>(offset) < _httpRequest._size) {
                const ssize_t count = sendfile(_sock, _httpResponse._fd, &offset, _httpRequest._size - offset);
                if (count < 0 && errno == EINTR) continue;
                if (count <= 0) break;
            }
        }
    }
};
