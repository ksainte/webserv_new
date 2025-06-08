#ifndef ERRORMESSAGES_HPP
# define ERRORMESSAGES_HPP
class ErrorMessages {
	public:
		static const char* HOST_NAME_NOT_FOUND;
		static const char* KEY_NOT_FOUND;
		static const char* SERVER_NAME_NOT_FOUND;
		static const char* E_SOCK_INIT;
		static const char* E_BAD_ARG;
		static const char* E_OPEN_FILE;
		static const char* E_ARGV;
		static const char* E_BAD_TYPE;
		static const char* E_BAD_ROUTE;
		static const char* E_TIMEOUT;
		static const char* E_BAD_PATH;
		static const char* E_BAD_METHOD;
		static const char* E_HTTP_VERSION;
		static const char* E_EPOLL_INIT;
		static const char* E_EPOLL_CTL_ADD;
		static const char* E_EPOLL_CTL_MOD;
		static const char* E_EPOLL_CTL_DEL;
		static const char* E_EPOLL_WAIT;
		static const char* E_INSERT_MAP;
		static const char* E_CONN_EXIST;
		static const char* E_ISS_CREATE;
		static const char* E_BAD_HEADER;
		static const char* E_HEADER_PARSE;
		static const char* E_RECV;
		static const char* E_HEADER_NOT_FOUND;
		static const char* E_HEADERS_END_NOT_FOUND;
		static const char* E_CONN_CLOSED;
		static const char* E_HEADERS_TOO_LONG;
		static const char* E_MAX_BODY_SIZE;
		static const char* E_FORK_FAILED;
};
#endif