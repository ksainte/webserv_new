#include "../inc/IEventHandler.hpp"
#include "../inc/Logger.hpp"
IEventHandler::~IEventHandler() 
{LOG_DEBUG << "IEventHandler destroyed\n";}