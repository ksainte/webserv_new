#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "IEventHandler.hpp"
#include "../inc/Request.hpp"
#include "../inc/Response.hpp"
// #include "../inc/ConnectionManager.hpp"  // <-- This is required!


class ConnectionManager;

class Connection : public virtual IEventHandler, public Request, public Response
{
	public:

	Connection();
	// ~Connection();

	void setManager(ConnectionManager* manager);
	void fillConnection(int clientFd, int sockFd);
	int	handleError() {return 1;}
	int	handleError1() {return 2;}
	int	handleEvent(const Event* p, int flags);

	private:

	int	_sockFd;
	int _clientFd;
	ConnectionManager* _manager;
	
};
#endif
