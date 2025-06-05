#include <csignal>
#include "../inc/Config.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Listener.hpp"
#include "../inc/Searcher.hpp"
#include "../inc/Tokenizer.hpp"
#include "../inc/utils.hpp"

void signalHandler(const int sigNum)
{
  if (sigNum == SIGINT 
	|| sigNum == SIGTERM
	|| sigNum == SIGQUIT)
  {
    // Recover a static bool
    bool& sigInt = getSigIntFlag();
    sigInt = true;
  }
}

int main()
{
  // Init signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGQUIT, signalHandler);
  
  try
  {
    const Tokenizer tokenizer("configFile/single-server.config");
    const Config config(tokenizer.ft_get_token_list());
    Searcher searcher(config);
    Epoll eventManager;
	ConnectionManager connManager(searcher, eventManager);
	Listener listener(searcher.getAddresses(), eventManager, connManager);
	eventManager.wait();
  }
  catch (const std::exception& e)
  {
    return 1;
  }
  
  return 0;
}
