#include "../inc/Config.hpp"
#include "../inc/Tokenizer.hpp"
#include "../inc/Listener.hpp"
#include "../inc/Searcher.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/utils.hpp"
#include <csignal>

void signalHander(int sigNum)
{
	if (sigNum == SIGINT)
	{
		// Recover a static bool 
		bool& sigInt = getSigIntFlag();
		sigInt = true;
	}
}

int main() {

	// Init sigInt handler
	signal(SIGINT, signalHander);
	try {

		Tokenizer tokenizer("configFile/single-server.config");
		Config config(tokenizer.ft_get_token_list());
		Searcher searcher(config);
		
		// Quick test to init socket with Listener
		std::list<IpPort> ipPortList;
		ipPortList.push_back(IpPort("127.0.0.2", 8080));

		EventManager eventManager;
		ConnectionManager connManager(searcher, eventManager);
		Listener listener(ipPortList, eventManager, connManager);
	
		eventManager.run();
	}
	catch (std::exception& e) {
		return 1;
	}
	return 0;
}