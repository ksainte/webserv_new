#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP
class Event;

class IEventHandler
{
public:
  virtual void handleError(int errnum) = 0;
  virtual int handleEvent(const Event* p, unsigned int flags) = 0;
  virtual ~IEventHandler();
};
#endif
