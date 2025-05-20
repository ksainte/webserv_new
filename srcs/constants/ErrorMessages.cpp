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

const char* ErrorMessages::E_BAD_IP =
"invalid ip address";

const char* ErrorMessages::E_BAD_TYPE =
"invalid type";

const char* ErrorMessages::E_BAD_ROUTE =
"invalid route";

const char* ErrorMessages::E_EPOLL_INIT =
"error during epoll instance initialization";

const char* ErrorMessages::E_EPOLL_CTL_ADD =
"error when registering new fd in epoll's interest list";

const char* ErrorMessages::E_EPOLL_CTL_MOD =
"error when modifying epoll's interest list";

const char* ErrorMessages::E_EPOLL_CTL_DEL =
"error when deleting new fd in epoll's interest list";

const char* ErrorMessages::E_EPOLL_WAIT =
"error when waiting for events";

const char* ErrorMessages::E_INSERT_MAP =
"error when inserting new map entry";

const char* ErrorMessages::E_CONN_EXIST =
"connexion already in use";