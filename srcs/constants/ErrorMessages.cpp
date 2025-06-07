#include "../../inc/constants/ErrorMessages.hpp"

const char* ErrorMessages::SERVER_NAME_NOT_FOUND = 
"server_name not configured";

const char* ErrorMessages::HOST_NAME_NOT_FOUND =
"Hostname not found inside the server configuration";

const char* ErrorMessages::KEY_NOT_FOUND =
"not found inside the configuration data structure";

const char* ErrorMessages::E_SOCK_INIT =
"error during socket initialization: ";

const char* ErrorMessages::E_BAD_ARG =
"invalid argument";

const char* ErrorMessages::E_BAD_TYPE =
"invalid type";

const char* ErrorMessages::E_BAD_ROUTE =
"invalid route";

const char* ErrorMessages::E_BAD_PATH =
  "invalid path";

const char* ErrorMessages::E_ARGV =
  "webserv: usage: ./webserv <file>";

const char* ErrorMessages::E_OPEN_FILE =
  "error opening configuration file";

const char* ErrorMessages::E_HTTP_VERSION =
  "HTTP version not supported";

const char* ErrorMessages::E_BAD_METHOD =
  "Method not allowed";

const char* ErrorMessages::E_EPOLL_INIT =
"error during epoll instance initialization";

const char* ErrorMessages::E_EPOLL_CTL_ADD =
"error when registering new fd in epoll's interest list";

const char* ErrorMessages::E_EPOLL_CTL_MOD =
"error when modifying epoll's interest list";

const char* ErrorMessages::E_EPOLL_CTL_DEL =
"error when deleting fd in epoll's interest list";

const char* ErrorMessages::E_EPOLL_WAIT =
"error when waiting for events";

const char* ErrorMessages::E_INSERT_MAP =
"error when inserting new map entry";

const char* ErrorMessages::E_CONN_EXIST =
"connexion already in use";

const char* ErrorMessages::E_ISS_CREATE =
  "error during input stream string creation";

const char* ErrorMessages::E_BAD_HEADER =
  "invalid header format";

const char* ErrorMessages::E_HEADER_PARSE =
  "error when parsing header: ";

const char* ErrorMessages::E_RECV =
  "recv failure: ";

const char* ErrorMessages::E_HEADERS_END_NOT_FOUND =
  "end of headers not found";

const char* ErrorMessages::E_CONN_CLOSED =
  "connection will be closed";

const char* ErrorMessages::E_HEADERS_TOO_LONG =
  "headers exceed maximum size";

const char* ErrorMessages::E_MAX_BODY_SIZE =
  "request body size exceed maximum";

const char* ErrorMessages::E_FORK_FAILED =
  "forking the child failed - closing current connection";