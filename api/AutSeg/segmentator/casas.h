#ifndef CASAS_H    // include guard
#define CASAS_H

///////////////////////////////////////////////////////////////////////////////
/// \class        Casas
/// \author       Brian L Thomas
/// \date         September 29, 2010
/// \brief        This class handles communication with CASAS Middleware.
///
/// A longer, complete description of the purpose for this class,
/// and how to use it.
///
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
//
#include <iostream>
#include <string>
#include <list>
#include <map>

// LOCAL INCLUDES
//
#include "callback.h"
#include "connection.h"

// TYPE DEFINITIONS
//
typedef void (*publishCallback)(std::map<std::string, std::string>);
typedef void (*directMsgCallback)(std::string, std::string);

class Casas: public CallBack
{
public:
	// LIFECYCLE

	/// Default Constructor
	/// \param   name is the name of the agent using this library.
	/// \returns Nothing
	Casas(const std::string& name);

	/// Constructor with Variables
	/// \param   name is the name of the agent using this library.
	/// \param   jid is the full jabber id to connect to the xmpp server.
	/// \param   password is used to authenticate the jid on the xmpp server.
	/// \returns Nothing
	Casas(const std::string& name, const std::string& jid,
			const std::string& password);

	/// Destructor
	/// \returns Nothing
	~Casas();

	// OPERATORS

	// OPERATIONS

	/// setJid
	/// \param   jid is the full jabber id to connect to the xmpp server.
	/// \returns Nothing
	void setJid(const std::string& jid);

	/// setPassword
	/// \param   password is used to authenticate the jid on the xmpp server.
	/// \returns Nothing
	void setPassword(const std::string& password);

	/// setCustomManager
	/// \param   manager
	/// \returns Nothing
	void setCustomManager(const std::string& manager);

	/// setChanMsgCallback
	/// \param   channel
	/// \param   pubCB
	/// \returns Nothing
	void setChanMsgCallback(const std::string& channel, publishCallback pubCB);

	/// setDirectMsgCallback
	/// \param   dirCB
	/// \returns Nothing
	void setDirectMsgCallback(directMsgCallback dirCB);

	/// connect
	/// \returns Nothing
	void connect();

	/// disconnect
	/// \returns Nothing
	void disconnect();

	/// addSubscribeChannel
	/// \param   channel
	/// \returns Nothing
	void addSubscribeChannel(const std::string& channel);

	/// removeSubscribeChannel
	/// \param   channel
	/// \returns Nothing
	void removeSubscribeChannel(const std::string& channel);

	/// addPublishChannel
	/// \param   channel
	/// \returns Nothing
	void addPublishChannel(const std::string& channel);

	/// removePublishChannel
	/// \param   channel
	/// \returns Nothing
	void removePublishChannel(const std::string& channel);

	/// connectedCallback
	/// \returns Nothing
	virtual void connectedCallback(void);

	/// subscribedCallback
	/// \param   message
	/// \returns Nothing
	virtual void subscribedCallback(const std::string& message);

	/// directMessageCallback
	/// \param   from
	/// \param   message
	/// \returns Nothing
	virtual void directMessageCallback(const std::string& from,
			const std::string& message);

	/// contactPresenceCallback
	/// \param   jid
	/// \param   presence
	/// \returns Nothing
	virtual void contactPresenceCallback(const std::string& jid,
			gloox::Presence::PresenceType presence);

	/// managerPresenceCallback
	/// \param   presence
	/// \returns Nothing
	virtual void
	managerPresenceCallback(gloox::Presence::PresenceType presence);

	/// isManager
	/// \param   manager
	/// \returns bool
	virtual bool isManager(const std::string& manager);

	/// log
	/// \param   message
	/// \returns Nothing
	void log(const std::string& message);

	/// debug
	/// \param   message
	/// \param   level
	/// \returns Nothing
	void debug(const std::string& message, int level = 0);

	/// send
	/// \param   to
	/// \param   message
	/// \returns Nothing
	void send(const std::string& to, const std::string& message);

	/// publish
	/// \param   message
	/// \param   channel
	/// \returns Nothing
	void publish(const std::string& message, const std::string& channel =
			"rawevent");

	/// publishData
	/// \param   by
	/// \param   type
	/// \param   serial
	/// \param   location
	/// \param   message
	/// \param   category
	/// \param   channel
	/// \returns Nothing
	void publishData(const std::string& by, const std::string& type,
			const std::string& serial, const std::string& location,
			const std::string& message, const std::string& category,
			const std::string& channel = "rawevent");

	/// tagEvent
	/// \param   event
	/// \param   label
	/// \param   value
	/// \param   channel
	/// \returns Nothing
	void tagEvent(std::map<std::string, std::string> event,
			const std::string& created_by, const std::string& experiment,
			const std::string& dataset, const std::string& label,
			const std::string& value = "", const std::string& channel = "tag");

	/// issuePrompt
	/// \param   created_by
	/// \param   label
	/// \param   value
	/// \param   channel
	/// \returns Nothing
	void issuePrompt(const std::string& created_by, const std::string& label,
			const std::string& value, const std::string& channel = "prompt");

	// INQUIRY

	/// getName
	/// \returns m_name
	const std::string& getName(void);

	/// getJid
	/// \returns m_jid
	const std::string& getJid(void);

	/// getPassword
	/// \returns m_password
	const std::string& getPassword(void);

	/// getManager
	/// \returns m_manager
	const std::string& getManager(void);

	/// getSubscribedChannels
	/// \returns m_chanSubscribe
	const std::list<std::string> getSubscribedChannels(void);

	/// getPublishedChannels
	/// \returns m_chanPublish
	const std::list<std::string> getPublishedChannels(void);

	/// isAuthenticated
	/// \returns bool
	bool isAuthenticated(void);

	/// printInfo
	/// \returns Nothing
	void printInfo(void);

private:
	/// getEvent
	/// \param   message
	/// \returns map of event from message.
	std::map<std::string, std::string> getEvent(const std::string& message);

	/// extractTag
	/// \param   message
	/// \param   tag
	/// \returns string
	std::string
	extractTag(const std::string& message, const std::string& tag);

	/// buildXmlField
	/// \param   obj
	/// \param   field
	/// \returns string
	std::string buildXmlField(std::map<std::string, std::string> obj,
			const std::string& field);

	/// getTimestamp
	/// \param   epoch
	/// \returns string
	std::string getTimestamp(const std::string& epoch);

	/// sendPubSub
	/// \param   action
	/// \param   who
	/// \param   channel
	/// \returns Nothing
	void sendPubSub(const std::string& action, const std::string& who,
			const std::string& channel);

	// MEMBER VARIABLES
	Connection* m_xmpp;
	std::string m_name;
	std::string m_jid;
	std::string m_password;
	std::string m_manager;
	bool m_managerOnline;
	std::list<std::string> m_chanSubscribe;
	std::list<std::string> m_chanPublish;
	std::map<std::string, publishCallback> m_chanMsgCallback;
	directMsgCallback m_directCallback;
	std::list<std::string> m_logBuffer;
};

#endif

