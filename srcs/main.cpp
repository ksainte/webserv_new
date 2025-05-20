#include "../inc/Config.hpp"
#include "../inc/Tokenizer.hpp"
#include "../inc/Listener.hpp"
#include "../inc/Searcher.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/utils.hpp"
#include <csignal>

// !----! 	REMOVE THIS BEFORE EVAL
#include <cstdlib>

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

		Tokenizer tokenizer("configFile/searchTest.config");
		Config config(tokenizer.ft_get_token_list());
		Searcher searcher(config);
		std::cout << "& of searcher is "<< &searcher;

		Listener listener(searcher.getAddresses(), searcher);

		char* configFileName = std::getenv("WEBSERV_CONF_FILE");
		if (configFileName)
		{
			std::ofstream os(configFileName);
			os << config.toJson(0) << std::endl;
			os.close();
		}
		const ConfigType::DirectiveValue* p;
		const char *test = searcher.getLocationPrefix(4, "2", "/contents");
		p = searcher.findLocationDirective(4, "root", "2", test);
		printf("%s\n", test);
		std::cout << " test is ------------------"<< test << "\n";
		if (p)
		{
			std::cout << "value :";
			for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) 
			{
					std::cout << *it << " ";
				}
			}
		listener.run();
	}
	catch (std::exception& e) {
		return 1;
	}
	return 0;
}

//1.ON A UN OBJECT LISTENER GENERAL QUI PREND EN ARG UNE LISTE DE IP / PORT! POUR CHAQUE PAIR IP/PORT->CREER NOUVEAU LISTENER
//2. FOR EACH IP/PORT CREATE A NEW TCP SOCKET
//3. FOR EACH NEW TCP SOCKET->CREATE AN EVENT THAT YOU WILL ADD TO EPOLL WAIT
//  3.1 THIS EVENT WILL HOLD THE TCP SOCKET AND THE & OF THE LISTENER
//4.WHEN EPOLL WAIT RETURNS A VALUE, THE UNDERLYING EVENT CAN BE EITHER A NEW CLIENT OR A CURRENT CONNECTION(READ REQUEST)
//  i.e THE UNDERLYING EVENT HAS THE LISTEN &, WE KNOW ITS A NEW CLIENT, IT IS A CONNECTION &, ITS A CURRENT CONNECTION
//5. EVENTREGISTER REGISTERS A NEW EVENT TO THE INSTANCE

// 1 EVENT FOR TCP SOCKET -> NEW CLIENT = NEW CONNECTION->REQUEST->RESPONSE
//   ONCE TCP SOCKET HANDLES ALL NEW CLIENTS= ALL NEW CONNECTIONS

//LISTEN ->

//A partir d un IP PORT ->

// int main() 
// {
// 	try {
				// RandomTokenList randTokenList;
		// randTokenList.generateServerBlock();
		// std::ofstream os1("tokensList.json", std::ios::trunc);
		// os1 << randTokenList.toJson() << std::endl;
		// os1.close();
		// LOG_INFO << "tokensList savconst ConfigType::DirectiveValue* p;
		// p = searcher.findLocationDirective("return", "test.example", "location");

		// if (p)
		// {
		// 	std::cout << "value :";
		// 	for (ConfigType::DirectiveValueIt it = (*p).begin(); it != (*p).end(); ++it) {
		// 		std::cout << *it << " ";
		// 	}
		// 	std::cout << std::endl;
		// }ed in tokensList.json";
		// Config config(randTokenList.getTokenList());
		// std::ofstream os2("config.json", std::ios::trunc);
		// LOG_INFO << "Configuration saved in config.json";
		// os2 << config.toJson(0) << std::endl;
		// os2.close();
		
		// Searcher searcher(config);
		// searcher.findServerDirective(1, "listen", "test");

		// EventManager eventManager;

		// Quick test to init socket with Listener
		// std::list<IpPort> ipPortList;
		// ipPortList.push_back(IpPort("127.0.0.1", 3000));

		// ConnectionManager connManager(eventManager);

		// Listener listener(ipPortList, eventManager, connManager);
		// eventManager.run();
// 	}
// 	catch (std::exception& e){
// 		return 1;
// 	}
	// List open file descriptor 
	// system("lsof -c webserv");
//	return 0;
// }