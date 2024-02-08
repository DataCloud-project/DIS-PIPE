#include <gloox/jid.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include "casas.h"
using namespace std;

// LIFECYCLE
Casas::Casas(const std::string& name) :
   m_xmpp(0)
{
   m_name = name;
   m_jid = "";
   m_password = "";
   m_manager = "";
   m_managerOnline = false;
   m_directCallback = NULL;
}

Casas::Casas(const std::string& name, const std::string& jid,
		const std::string& password) :
   m_xmpp(0)
{
   m_name = name;
   m_jid = jid;
   m_password = password;
   gloox::JID j(jid);
   m_manager = "manager@" + j.server();
   m_managerOnline = false;
   m_directCallback = NULL;
}

Casas::~Casas()
{
	delete m_xmpp;
}

// OPERATORS

// OPERATIONS
void Casas::setJid(const std::string& jid)
{
	if (!isAuthenticated())
	{
		m_jid = jid;
		gloox::JID j(jid);
		m_manager = "manager@" + j.server();
	}
}

void Casas::setPassword(const std::string& password)
{
	if (!isAuthenticated())
	{
		m_password = password;
	}
}

void Casas::setCustomManager(const std::string& manager)
{
	if (!isAuthenticated())
	{
		m_manager = manager;
	}
}

//void Casas::setMsgCallback(void(*msgCB)(std::map<std::string, std::string>))
//{
//	m_msgCallback = msgCB;
//}

void Casas::setChanMsgCallback(const std::string& channel,
		publishCallback pubCB)
{
	m_chanMsgCallback[channel] = pubCB;
}

void Casas::setDirectMsgCallback(directMsgCallback dirCB)
{
	m_directCallback = dirCB;
}

void Casas::connect()
{
	if (m_jid != "" && m_password != "")
	{
		m_xmpp = new Connection(m_jid, m_password);
		m_xmpp->registerCallback(this);
		m_xmpp->start();
	}
}

void Casas::disconnect()
{
	m_xmpp->stop();
}

void Casas::addSubscribeChannel(const std::string& channel)
{
	bool found = false;
	list<string>::iterator it;
	for (it = m_chanSubscribe.begin(); it != m_chanSubscribe.end(); it++)
	{
		if (*it == channel)
			found = true;
	}
	if (!found)
	{
		m_chanSubscribe.push_back(channel);
		sendPubSub("register", "subscriber", channel);
	}
}

void Casas::removeSubscribeChannel(const std::string& channel)
{
	bool found = false;
	list<string>::iterator it;
	for (it = m_chanSubscribe.begin(); it != m_chanSubscribe.end(); it++)
	{
		if (*it == channel)
			found = true;
	}
	if (found)
	{
		m_chanSubscribe.remove(channel);
		sendPubSub("unregister", "subscriber", channel);
	}
}

void Casas::addPublishChannel(const std::string& channel)
{
	bool found = false;
	list<string>::iterator it;
	for (it = m_chanPublish.begin(); it != m_chanPublish.end(); it++)
	{
		if (*it == channel)
			found = true;
	}
	if (!found)
	{
		m_chanPublish.push_back(channel);
		sendPubSub("register", "publisher", channel);
	}
}

void Casas::removePublishChannel(const std::string& channel)
{
	bool found = false;
	list<string>::iterator it;
	for (it = m_chanPublish.begin(); it != m_chanPublish.end(); it++)
	{
		if (*it == channel)
			found = true;
	}
	if (found)
	{
		m_chanPublish.remove(channel);
		sendPubSub("unregister", "publisher", channel);
	}
}

void Casas::connectedCallback(void)
{
	cout << "CASAS C++ Library is Connected!" << endl;
	addPublishChannel("log");
	managerPresenceCallback(gloox::Presence::Available);
	log("Online");
}

void Casas::subscribedCallback(const std::string& message)
{
	map<string, string> event = getEvent(message);
	if (m_chanMsgCallback.find(event["channel"]) != m_chanMsgCallback.end())
	{
		if (m_chanMsgCallback[event["channel"]])
		{
			m_chanMsgCallback[event["channel"]](event);
		}
	}
	else
	{
		directMessageCallback(m_manager, message);
	}
}

void Casas::directMessageCallback(const std::string& from,
		const std::string& message)
{
	if (m_directCallback)
	{
		m_directCallback(from, message);
	}
}

void Casas::contactPresenceCallback(const std::string& jid,
		gloox::Presence::PresenceType presence)
{
	if (jid == m_manager)
	{
		managerPresenceCallback(presence);
	}
}

void Casas::managerPresenceCallback(gloox::Presence::PresenceType presence)
{
	list<string>::iterator it;
	switch (presence)
	{
	case gloox::Presence::Available:
		m_managerOnline = true;
		for (it = m_chanSubscribe.begin(); it != m_chanSubscribe.end(); it++)
		{
			sendPubSub("register", "subscriber", *it);
		}
		for (it = m_chanPublish.begin(); it != m_chanPublish.end(); it++)
		{
			sendPubSub("register", "publisher", *it);
		}
		for (it = m_logBuffer.begin(); it != m_logBuffer.end(); it++)
		{
			log(*it);
		}
		break;
	default:
		m_managerOnline = false;
	}
}

bool Casas::isManager(const std::string& manager)
{
	if (manager == m_manager)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

void Casas::log(const std::string& message)
{
	if (m_managerOnline)
	{
		publishData(m_name, "system", m_name, "system", message, "system",
				"log");
	}
	else
	{
		m_logBuffer.push_back(message);
	}
}

void Casas::debug(const std::string& message, int level)
{
	publishData(m_name, "system", m_name, "system", message, "system", "debug");
}

void Casas::send(const std::string& to, const std::string& message)
{
	if (isAuthenticated())
	{
		m_xmpp->sendMessage(to, message);
	}
}

void Casas::publish(const std::string& message, const std::string& channel)
{
	string packet = "<publish><channel>" + channel + "</channel><data>";
	packet += message + "</data></publish>";
	send(m_manager, packet);
}

void Casas::publishData(const std::string& by, const std::string& type,
		const std::string& serial, const std::string& location,
		const std::string& message, const std::string& category,
		const std::string& channel)
{
	string xml = "<event><by>" + by;
	xml += "</by><type>" + type;
	xml += "</type><serial>" + serial;
	xml += "</serial><location>" + location;
	xml += "</location><message>" + message;
	xml += "</message><category>" + category;
	xml += "</category></event>";

	publish(xml, channel);
}

void Casas::tagEvent(std::map<std::string, std::string> event,
		const std::string& created_by, const std::string& experiment,
		const std::string& dataset, const std::string& label,
		const std::string& value, const std::string& channel)
{
	string xml = "<tag>";
	xml += "<created_by>" + created_by + "</created_by>";
	xml += "<label><name>" + label + "</name>";
	xml += "<value>" + value + "</value></label>";
	xml += "<dataset><name>" + dataset + "</name></dataset>";
	xml += "<experiment><name>" + experiment + "</name></experiment>";
	xml += "</tag>";

	xml += "<event>";
	xml += buildXmlField(event, "by");
	xml += buildXmlField(event, "type");
	xml += buildXmlField(event, "serial");
	xml += buildXmlField(event, "location");
	xml += buildXmlField(event, "message");
	xml += buildXmlField(event, "epoch");
	xml += buildXmlField(event, "uuid");
	xml += buildXmlField(event, "category");
	xml += "</event>";

	publish(xml, channel);
}

void Casas::issuePrompt(const std::string& created_by, const std::string& label,
		const std::string& value, const std::string& channel)
{
	stringstream epoch;
	epoch << time(NULL) << ".0";

	string xml = "<prompt>";
	xml += "<created_by>" + created_by + "</created_by>";
	xml += "<label><name>" + label + "</name>";
	xml += "<value>" + value + "</value></label>";
	xml += "</prompt>";

	xml += "<event>";
	xml+= "<by>Prompter</by>";
	xml += "<type>Prompt</type>";
	xml += "<serial>system</serial>";
	xml += "<location>Prompter</location>";
	xml += "<message>prompt</message>";
	xml += "<category>system</category>";
	xml += "<epoch>" + epoch.str() + "</epoch>";
	xml += "</event>";

	publish(xml, channel);
}

// INQUIRY
const std::string& Casas::getJid(void)
{
	return(m_jid);
}

const std::string& Casas::getPassword(void)
{
	return(m_password);
}

const std::string& Casas::getManager(void)
{
	return(m_manager);
}

const std::list<std::string> Casas::getSubscribedChannels(void)
{
	return(m_chanSubscribe);
}

const std::list<std::string> Casas::getPublishedChannels(void)
{
	return(m_chanPublish);
}

bool Casas::isAuthenticated(void)
{
	if (m_xmpp)
	{
		return(m_xmpp->isAuthenticated());
	}
	else
	{
		return(false);
	}
}

void Casas::printInfo(void)
{
	cout << "name:     " << m_name << endl;
	cout << "jid:      " << m_jid << endl;
	cout << "password: " << m_password << endl;
	cout << "manager:  " << m_manager << endl;
	cout << "channels subscribed:" << endl;
	list<string>::iterator it;
	for (it = m_chanSubscribe.begin(); it != m_chanSubscribe.end(); it++)
	{
		cout << "    " << *it << endl;
	}
	cout << "channels published:" << endl;
	for (it = m_chanPublish.begin(); it != m_chanPublish.end(); it++)
	{
		cout << "    " << *it << endl;
	}
}

std::map<std::string, std::string> Casas::getEvent(const std::string& message)
{
	map<string, string> packet;
	string tmp;
	if (extractTag(message, "event") != "")
	{
		packet["by"] = extractTag(message, "by");
		packet["type"] = extractTag(message, "type");
		packet["serial"] = extractTag(message, "serial");
		packet["location"] = extractTag(message, "location");
		packet["message"] = extractTag(message, "message");
		packet["category"] = extractTag(message, "category");
		packet["epoch"] = extractTag(message, "epoch");
		packet["uuid"] = extractTag(message, "uuid");
		packet["stamp"] = getTimestamp(packet["epoch"]);
		packet["channel"] = extractTag(message, "channel");
	}
	//if (extractTag(message, "tag") != "")
	//{
	//packet["label"] = extractTag(message, "label");
	//packet["value"] = extractTag(message, "value");
	//}
	packet["created_by"] = extractTag(message, "created_by");
	tmp = extractTag(message, "label");
	packet["label"] = extractTag(tmp, "name");
	packet["value"] = extractTag(tmp, "value");
	tmp = extractTag(message, "dataset");
	packet["dataset"] = extractTag(tmp, "name");
	tmp = extractTag(message, "experiment");
	packet["experiment"] = extractTag(tmp, "name");

	return(packet);
}

string Casas::extractTag(const std::string& message, const std::string& tag)
{
	string result = "";
	size_t pos1, pos2, len;
	pos1 = message.find("<" + tag + ">");
	if (pos1 != string::npos)
	{
		pos2 = message.find("</" + tag + ">");
		len = pos2 - (pos1 + 2 + tag.size());
		result = message.substr(pos1 + 2 + tag.size(), len);
	}

	return(result);
}

std::string Casas::buildXmlField(std::map<std::string, std::string> obj,
		const std::string& field)
{
	string xml = "";
	if (obj.find(field) != obj.end())
	{
		xml += "<" + field + ">";
		xml += obj[field];
		xml += "</" + field + ">";
	}
	return(xml);
}

std::string Casas::getTimestamp(const std::string& epoch)
{
	time_t tTime = atoi(epoch.c_str());
	size_t point = epoch.find(".");
	string milSec = epoch.substr(point + 1);
	struct tm *tmPtr = localtime(&tTime);
	char buffer[25];
	strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", tmPtr);
	string final = buffer;
	final += "." + milSec;
	return(final);
}

void Casas::sendPubSub(const std::string& action, const std::string& who,
		const std::string& channel)
{
	string message = "<system><do>" + action + "</do><as>" + who;
	message += "</as><channel>" + channel + "</channel></system>";
	send(m_manager, message);
}

