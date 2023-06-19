#ifndef CALLBACK_H
#define CALLBACK_H

#include <string>
#include <gloox/presence.h>

class CallBack
{
public:
	virtual ~CallBack()
	{
	}
	virtual void connectedCallback(void) = 0;
	virtual void subscribedCallback(const std::string& message) = 0;
	virtual void directMessageCallback(const std::string& from,
			const std::string& message) = 0;
	virtual void contactPresenceCallback(const std::string& jid,
			gloox::Presence::PresenceType presence) = 0;
	virtual void
	managerPresenceCallback(gloox::Presence::PresenceType presence) = 0;
	virtual bool isManager(const std::string& manager) = 0;
};

#endif

