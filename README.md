# HTTP Server

An educational Linux C++11 HTTP server with a six-worker pthread pool, static file responses, and optional CGI demonstrations. The implementation supports one GET or POST request per connection.

## Build and run

```sh
sudo apt-get install build-essential cmake
cmake -S . -B build
cmake --build build --parallel
./build/Httpserver 9090
```

Linux is required for the socket and sendfile implementation. Open http://localhost:9090/. CMake copies English pages to `build/wwwroot/` and builds CGI programs there. Paths no longer depend on the launch directory. `build/build.sh` is a convenience wrapper accepting additional CMake options.

## Optional MySQL login demonstration

```sh
sudo apt-get install pkg-config default-libmysqlclient-dev
cmake -S . -B build -DENABLE_MYSQL_CGI=ON
cmake --build build --parallel
export MYSQL_HOST=127.0.0.1
export MYSQL_USER=webserver
export MYSQL_PASSWORD='your-local-database-password'
export MYSQL_DATABASE=webserver
./build/Httpserver 9090
```

Create the database and a `user` table with `username VARCHAR(50)` and `passwd VARCHAR(50)` columns. The demo uses the submitted username, validates query escaping through the MySQL client, fails closed on connection/query errors, and does not print passwords. It stores plaintext passwords for educational compatibility and serves a result page; it has no authenticated session layer. Replace this design with hashed passwords and sessions before implementing real accounts.

Without `ENABLE_MYSQL_CGI`, the static server and network diagnostics remain usable; the database CGI route is unavailable.

## Network diagnostics

Install Nmap at `/usr/bin/nmap` to use `/penetration.html`. The CGI accepts exactly one IPv4 address and one port from 1 to 65535, invokes Nmap without a shell, and honors the selected port. The page displays output as text, shows request failures, and disables duplicate submissions. Use it only for hosts you are authorized to test.

The previous unsupported tool selector and missing tool-addition page have been removed. Only the implemented Nmap operation is offered.

## Request and process handling

- Request lines are capped at 8 KiB and headers at 100 lines.
- POST bodies require a numeric Content-Length and are capped at 1 MiB.
- Accepted sockets have ten-second read/write timeouts.
- Static paths reject traversal and encoded path tricks.
- Only `CgiTool` and `CgiMySQL` are executable routes.
- CGI processes receive isolated pipes and explicit request environment variables through posix_spawn.
- CGI response waits use a ten-second idle timeout and a 1 MiB output cap.
- Parent workers wait for their own CGI child and transmit partial socket/file writes correctly.

## Structure

- `src/main.cpp`: validated command-line port.
- `bin/`: TCP listener, HTTP server, shared includes, and logging.
- `threadpool/`: workers and per-connection tasks.
- `httpmsg/`: line reading, request parsing, response building, and CGI handling.
- `lock/`: pthread synchronization wrappers.
- `root/`: English static pages and CGI source files.

## Verify

```sh
python3 tests_smoke.py
```

Build before running the smoke checks. They start a local server, verify normal, missing, traversal, invalid-length, and large responses, and reject malformed CGI parameters without scanning a network target. Linux CI builds the server and runs the same checks. MySQL integration additionally requires a configured test database.

This is a learning project, with no TLS, full HTTP/1.1 keep-alive/pipelining, chunked request decoding, or production authentication. The listener binds all interfaces; restrict access appropriately when using the optional diagnostics demo. No benchmark-backed performance claim or explicit project license is currently provided. Retained third-party headers keep their upstream notices.
