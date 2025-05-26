#include <csignal>
#include "../inc/Config.hpp"
#include "../inc/ConnectionManager.hpp"
#include "../inc/Listener.hpp"
#include "../inc/Searcher.hpp"
#include "../inc/Tokenizer.hpp"
#include "../inc/utils.hpp"

void signalHandler(const int sigNum)
{
  if (sigNum == SIGINT)
  {
    // Recover a static bool
    bool& sigInt = getSigIntFlag();
    sigInt = true;
  }
}

int main()
{
  // Init sigInt handler
  signal(SIGINT, signalHandler);
  try
  {
    const Tokenizer tokenizer("configFile/cgi.config");
    const Config config(tokenizer.ft_get_token_list());
    Searcher searcher(config);
    EventManager eventManager;
    ConnectionManager connManager(searcher, eventManager);
    Listener listener(searcher.getAddresses(), eventManager, connManager);

    eventManager.run();
  }
  catch (std::exception& e)
  {
    return 1;
  }
  return 0;
}
