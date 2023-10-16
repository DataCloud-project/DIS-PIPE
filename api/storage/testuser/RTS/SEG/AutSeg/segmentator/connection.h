#ifndef CONNECTION_H    // include guard
#define CONNECTION_H

///////////////////////////////////////////////////////////////////////////////
/// \class        Connection
/// \author       Brian L Thomas
/// \date         September 29, 2010
/// \brief        This class manages a connection to an XMPP server.
///
/// A longer, complete description of the purpose for this class,
/// and how to use it.
///
/// REVISION HISTORY:
///
/// 28 September 2010      Creation. -Brian L Thomas
///
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
//
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/messagesession.h>
#include <gloox/presencehandler.h>
#include <gloox/rosteritem.h>
#include <gloox/rostermanager.h>
#include <gloox/connectionlistener.h>
#include <string>
#include <list>

// LOCAL INCLUDES
//
#include "listener.h"
#include "callback.h"

class Connection: public gloox::MessageHandler,
		public gloox::ConnectionListener,
		public DefaultRosterListener
{
public:
	// LIFECYCLE

	/// Default Constructor
	/// \returns Nothing
	Connection();

	/// Constructor with Variables
	/// \returns Nothing
	Connection(std::string jid, std::string password);

	/// Destructor
	/// \returns Nothing
	~Connection();

	// OPERATORS

	// OPERATIONS
	void registerCallback(CallBack* cb);

	/// start
	/// \returns Nothing
	void start();

	/// stop
	/// \returns Nothing
	void stop();

	void setDebug(bool debug);

	/// sendMessage
	/// \param   to is the JID the message body will be sent to.
	/// \param   body is the message that will be sent.
	/// \returns Nothing
	void sendMessage(const gloox::JID& to, const std::string body);
	void sendMessage(const std::string to, const std::string body);

	/// handleMessage
	/// \param   msg is the gloox::Message object that was received.
	/// \param   session is the gloox::MessageSession we are ignoring.
	/// \returns Nothing
	virtual void handleMessage(const gloox::Message& msg,
			gloox::MessageSession* session = 0);

	/// handleRosterPresence
	/// \param   item is the gloox::RosterItem with the presence.
	/// \param   resource is the user's resource.
	/// \param   presence is the gloox::Presence::PresenceType.
	/// \param   msg is the presence message.
	/// \returns Nothing
	virtual void handleRosterPresence(const gloox::RosterItem & item,
			const std::string& resource,
			gloox::Presence::PresenceType presence, const std::string& msg);

	/// handleSelfPresence
	/// \param   item is gloox::RosterItem for myself.
	/// \param   resource is my resource.
	/// \param   presence is my presence.
	/// \param   msg is what my presence is.
	/// \returns Nothing
	virtual void handleSelfPresence(const gloox::RosterItem& item,
			const std::string& resource,
			gloox::Presence::PresenceType presence, const std::string& msg)
	{
	}

	/// handleNonrosterPresence
	/// \returns Nothing
	virtual void handleNonrosterPresence(const gloox::Presence& presence)
	{
	}

	/// handleRosterError
	/// \returns Nothing
	virtual void handleRosterError(const gloox::IQ& iq)
	{
	}

	/// onConnect
	/// \returns Nothing
	virtual void onConnect();

	/// onDisconnect
	/// \returns Nothing
	virtual void onDisconnect(gloox::ConnectionError e);

	/// onResourceBindError
	/// \returns Nothing
	virtual void onResourceBindError(gloox::ResourceBindError error);

	/// onSessionCreateError
	/// \returns Nothing
	virtual void onSessionCreateError(gloox::SessionCreateError error);

	/// onTLSConnect
	/// \returns true
	virtual bool onTLSConnect(const gloox::CertInfo &info)
	{
		return true;
	}

	/// onStreamEvent
	/// \returns Nothing
	virtual void onStreamEvent(gloox::StreamEvent event)
	{
	}

	void handleSubscription(const std::string& jid);

	// INQUIRY

	/// printInfo
	/// \returns Nothing
	void printInfo(void);
	bool isAuthenticated(void);

protected:
	CallBack* m_callback;

private:
	// MEMBER VARIABLES
	gloox::Client* m_Client;
	gloox::RosterManager* m_Roster;
	std::string m_jid;
	std::string m_password;
	bool m_authenticated;
	bool m_debug;
};

#endif

