#ifndef ERRORMESSAGES_HPP
# define ERRORMESSAGES_HPP
class ErrorMessages {
	public:
		static const char* HOST_NAME_NOT_FOUND;
		static const char* KEY_NOT_FOUND;
		static const char* SERVER_NAME_NOT_FOUND;
		static const char* E_SOCK_INIT;
		static const char* E_BAD_ARG;
		static const char* E_BAD_IP;
		static const char* E_BAD_TYPE;
		static const char* E_BAD_ROUTE;
		static const char* E_EPOLL_INIT;
		static const char* E_EPOLL_CTL_ADD;
		static const char* E_EPOLL_CTL_MOD;
		static const char* E_EPOLL_CTL_DEL;
		static const char* E_EPOLL_WAIT;
		static const char* E_INSERT_MAP;
		static const char* E_CONN_EXIST;
};
#endif