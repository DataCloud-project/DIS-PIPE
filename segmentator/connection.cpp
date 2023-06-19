#include "casas.h"
#include <gloox/client.h>
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace gloox;

Connection::Connection() :
	m_Client(0), m_Roster(0)
{
	m_authenticated = false;
	m_callback = 0;
}

Connection::Connection(string jid, string password) :
	m_Client(0), m_Roster(0)
{
	m_jid = jid;
	m_password = password;
	m_authenticated = false;
	m_callback = 0;
}

Connection::~Connection()
{
	delete m_Client;
	m_callback = 0;
}

void Connection::registerCallback(CallBack* cb)
{
	m_callback = cb;
}

void Connection::start()
{
	JID jid(m_jid);

	m_Client = new Client(jid, m_password);
	m_Client->registerMessageHandler(this);
	m_Client->registerConnectionListener(this);

	m_Roster = m_Client->rosterManager();
	m_Roster->registerRosterListener(this);

	m_Client->connect(); // this function returns only in case of failure.

	delete m_Client;
	m_Client = 0;
	throw runtime_error("Failed connection attempt to server.");
}

void Connection::stop()
{
	m_authenticated = false;
	m_Client->disconnect();
}

void Connection::setDebug(bool debug)
{
	m_debug = debug;
}

void Connection::sendMessage(const gloox::JID& to, const std::string body)
{
	Message msg(Message::Chat, to, body);
	m_Client->send(msg);
}

void Connection::sendMessage(const std::string to, const std::string body)
{
	JID to_jid(to);
	sendMessage(to_jid, body);
}

void Connection::handleMessage(const Message& msg, MessageSession* session)
{
	string from = msg.from().bare();
	switch (msg.subtype())
	{
	case Message::Chat:
	case Message::Normal:
		if (msg.body() != "")
		{
			if (m_callback)
			{
				if (m_callback->isManager(from))
				{
					m_callback->subscribedCallback(msg.body());
				}
				else
				{
					m_callback->directMessageCallback(from, msg.body());
				}
			}
		}
		break;
	case Message::Error:
		cout << "Error message" << endl;
		break;
	default:
		cout << "default value message" << endl;
	}
}

void Connection::handleRosterPresence(const RosterItem& item,
		const std::string& resource, Presence::PresenceType presence,
		const std::string& msg)
{
	string user = item.jid();
	string status;

	switch (presence)
	{
	case Presence::Available:
		status = "available";
	case Presence::Unavailable:
		status = "unavailable";
		if (m_callback)
		{
			if (m_callback->isManager(user))
			{
				m_callback ->managerPresenceCallback(presence);
			}
			else
			{
				m_callback->contactPresenceCallback(user, presence);
			}
		}
		break;
	case Presence::Chat:
		status = "chat";
		break;
	case Presence::Away:
		status = "away";
		break;
	case Presence::DND:
		status = "dnd";
		break;
	case Presence::XA:
		status = "xa";
		break;
	default:
		status = "unknown";
	}

	cout << "New presence - " << user << " is " << status << " (" << msg
			<< ")\n";
}

void Connection::onConnect()
{
	m_authenticated = true;
	cout << "Authenticated to the Jabber server" << endl;
	handleSubscription("manager@localhost");

	if (m_callback)
	{
		m_callback->connectedCallback();
	}
}

void Connection::onDisconnect(ConnectionError e)
{
	if (e != gloox::ConnNoError)
	{
		if (e == gloox::ConnStreamClosed)
			throw runtime_error("Connection closed by server.");
		else if (e == gloox::ConnParseError)
			throw runtime_error("An XML parse error occurred.");
		else if (e == gloox::ConnConnectionRefused)
			throw runtime_error(
					"The connection was refused by the server (on the socket level).");
		else if (e == gloox::ConnDnsError)
			throw runtime_error("Resolving the server's hostname failed.");
		else if (e == gloox::ConnAuthenticationFailed)
			throw runtime_error(
					"Authentication failed. Username/password wrong or account does not exist.");
		else
			// to lazy to write other descriptions
			throw runtime_error("Connection error");
	}
}

void Connection::onResourceBindError(ResourceBindError error)
{
	throw runtime_error("ResourceBindError");
}

void Connection::onSessionCreateError(SessionCreateError error)
{
	throw runtime_error("SessionCreateError");
}

void Connection::handleSubscription(const std::string& jid)
{
	gloox::JID cJid(jid);
	gloox::RosterItem* rItem = m_Roster->getRosterItem(cJid);
	if (rItem)
	{
		//cout << "RosterItem info..." << endl;
		//cout << "jid: " << rItem->jid() << endl;
		//cout << "online:" << rItem->online() << endl;
		gloox::SubscriptionType subType = rItem->subscription();
		//cout << "SubscriptionType: ";
		switch (subType)
		{
		case gloox::S10nNone:
			// Contact and user are not subscribed to each other, and neither
			// has requested a subscription from the other.
			m_Roster->subscribe(cJid);
			m_Roster->synchronize();
			break;
		case gloox::S10nNoneOut:
			// Contact and user are not subscribed to each other, and user has
			// sent contact a subscription request but contact has not replied
			// yet.
			break;
		case gloox::S10nNoneIn:
			// Contact and user are not subscribed to each other, and contact
			// has sent user a subscription request but user has not replied
			// yet (note: contact's server SHOULD NOT push or deliver roster
			// items in this state, but instead SHOULD wait until contact has
			// approved subscription request from user).
		case gloox::S10nNoneOutIn:
			// Contact and user are not subscribed to each other, contact has
			// sent user a subscription request but user has not replied yet,
			// and user has sent contact a subscription request but contact has
			// not replied yet.
			m_Roster->ackSubscriptionRequest(cJid, true);
			m_Roster->synchronize();
			break;
		case gloox::S10nTo:
			// User is subscribed to contact (one-way).
			break;
		case gloox::S10nToIn:
			// User is subscribed to contact, and contact has sent user a
			// subscription request but user has not replied yet.
			m_Roster->ackSubscriptionRequest(cJid, true);
			m_Roster->synchronize();
			break;
		case gloox::S10nFrom:
			// Contact is subscribed to user (one-way).
			m_Roster->subscribe(cJid);
			m_Roster->synchronize();
			break;
		case gloox::S10nFromOut:
			// Contact is subscribed to user, and user has sent contact a
			// subscription request but contact has not replied yet.
			break;
		case gloox::S10nBoth:
			// User and contact are subscribed to each other (two-way).
			break;
		default:
			break;
		}
	}
	else
	{
		m_Roster->subscribe(cJid);
		m_Roster->synchronize();
	}
}

/// printInfo
/// \returns Nothing
void Connection::printInfo(void)
{
	cout << "Jid: " << m_jid << endl;
	cout << "isAuthenticated: " << endl;
}

bool Connection::isAuthenticated()
{
	return(m_authenticated);
}

