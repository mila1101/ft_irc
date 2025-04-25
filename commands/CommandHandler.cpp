/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:33:36 by eahn              #+#    #+#             */
/*   Updated: 2025/04/25 15:00:35 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../utils/Logger.hpp"
#include "../client/Channel.hpp"
#include "../client/Client.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server &server) : server_(server)
{
	addCommand("CAP", &CommandHandler::cmdCap);
	addCommand("PASS", &CommandHandler::cmdPass);
	addCommand("NICK", &CommandHandler::cmdNick);
	addCommand("USER", &CommandHandler::cmdUser);
	addCommand("JOIN", &CommandHandler::cmdJoin);
	addCommand("PRIVMSG", &CommandHandler::cmdMsg);
	addCommand("QUIT", &CommandHandler::cmdQuit);
	addCommand("PING", &CommandHandler::cmdPing);
	addCommand("PONG", &CommandHandler::cmdPong);
	addCommand("TOPIC", &CommandHandler::cmdTopic);
	addCommand("KICK", &CommandHandler::cmdKick);
	addCommand("INVITE", &CommandHandler::cmdInvite);
	addCommand("MODE", &CommandHandler::cmdMode);
	addCommand("PART", &CommandHandler::cmdPart);
}

CommandHandler::~CommandHandler() {}

void CommandHandler::addCommand(const std::string& name, CommandFunc handler)
{
	commandTable[name] = std::bind(handler, this, std::placeholders::_1, std::placeholders::_2);
}

void CommandHandler::dispatch(int clientFd, const ParsedCommand& cmd)
{
	if (cmd.command.empty()) {
		Logger::warning("Empty command received from fd=" + std::to_string(clientFd));
		return;
	}

	auto it = commandTable.find(cmd.command);
	if (it != commandTable.end()) {
		Logger::info("Dispatching command: " + cmd.command + " from fd=" + std::to_string(clientFd));
		it->second(clientFd, cmd.params);
	} else {
		Logger::warning("Unknown command: " + cmd.command + " from fd=" + std::to_string(clientFd));
	}
}

void CommandHandler::cmdCap(int clientFd, const std::vector<std::string>& params)
{
    if (params.empty()) {
        return;
    }

    if (params[0] == "LS") {
        server_.msgClient(clientFd, "CAP * LS :");
    }
    else if (params[0] == "END") {
        return;
    }
}

void CommandHandler::cmdPass(int fd, const std::vector<std::string>& params)
{
    std::map<int, Client>& clients = server_.getClients();
    if (clients.find(fd) == clients.end())
        return;

    Client& client = clients[fd];

    if (client.isRegistered())
    {
        server_.msgClient(fd, "462 :You may not reregister");
        return;
    }

    if (params.empty())
    {
        server_.msgClient(fd, "461 PASS :Not enough parameters");
        return;
    }

    if (params[0] != server_.getPassword())
    {
        server_.msgClient(fd, "464 :Password incorrect");
        return;
    }

    client.setPasswordVerified(true);
    Logger::info("Password verified for fd=" + std::to_string(fd));
}

void CommandHandler::cmdNick(int clientFd, const std::vector<std::string>& params)
{
	std::map<int, Client>& clients = server_.getClients();
	const std::map<std::string, Channel>& channels = server_.getChannels();

	if (params.empty())
	{
		Logger::warning("NICK: no nickname provided from fd=" + std::to_string(clientFd));
		server_.msgClient(clientFd, ERR_NONICKNAMEGIVEN(server_.getIP()));
		return;
	}
	std::string newNick = params[0];
	std::regex pattern("^[a-zA-Z][a-zA-Z0-9\\-_]{0,15}$");

	if (!std::regex_match(newNick, pattern))
	{
		server_.msgClient(clientFd, ERR_ERRONEUSNICKNAME(server_.getIP(), newNick));
		return;
	}

	for (const auto& pair : clients)
	{
		if (pair.second.getNickName() == newNick && pair.first != clientFd)
		{
			server_.msgClient(clientFd, ERR_NICKNAMEINUSE(server_.getIP(), newNick));
			return;
		}
	}

	Client& client = clients[clientFd];
	std::string oldNick = client.getNickName();

	if (oldNick != newNick)
	{
		std::string userMsg;
		if (client.isRegistered())
			userMsg = ":" + oldNick + "!" + client.getUserName() + "@" + server_.getIP();
		else
			userMsg = ":" + server_.getIP() + " NICK :" + newNick + "\r";

		client.setNickName(newNick);

		std::set<int> notifiedClients;
		for (const auto& channel : channels)
		{
			if (channel.second.isMember(clientFd))
			{
				for (int memberFd : channel.second.getMembers())
				{
					if (notifiedClients.insert(memberFd).second)
					{
						server_.msgClient(memberFd, userMsg);
					}
				}
			}
		}
		if (notifiedClients.find(clientFd) == notifiedClients.end())
			server_.msgClient(clientFd, userMsg);
	}
}


void CommandHandler::cmdUser(int fd, const std::vector<std::string>& params)
{
    std::map<int, Client>& clients = server_.getClients();

    if (clients.find(fd) == clients.end())
        return;

    Client& client = clients[fd];
    if (client.isRegistered())
    {
        server_.msgClient(fd, ERR_ALREADYREGISTRED(client.getNickName()));
        return;
    }

    if (params.size() < 4)
    {
        server_.msgClient(fd, ERR_NEEDMOREPARAMS(client.getNickName(), "USER"));
        return;
    }
    std::string username = params[0];
    std::string realname = params[3];
    if (!realname.empty() && realname[0] == ':')
        realname = realname.substr(1);
    client.setUserName(username);
    client.setRealName(realname);

	if (!client.getNickName().empty() && client.isPasswordVerified())
	{
		client.setRegistered(true);
		server_.sendWelcome(fd, client);
	}
}

void CommandHandler::cmdJoin(int fd, const std::vector<std::string>& params)
{
	if (params.empty() || params[0] == ":")
	{
		server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getServerName(), "JOIN"));
		return;
	}
	std::string channelName = params[0];
	std::string password = (params.size() > 1) ? params[1] : "";

	if (channelName[0] != '#')
		channelName = "#" + channelName;

	//Create channel
	if (server_.getChannels().find(channelName) == server_.getChannels().end())
	{
		server_.getChannels().emplace(channelName, Channel(channelName));
		Logger::info("Channel " + channelName + " created by " + server_.getClient(fd).getNickName());
	}

	Channel& channel = server_.getChannels()[channelName];
	bool isFirstMember = channel.getMembers().empty();

	// Restrictions
	if (!channel.getPassword().empty() && password != channel.getPassword())
	{
		server_.msgClient(fd, ERR_BADCHANNELKEY(server_.getIP(), channelName));
		return;
	}
	if (channel.isInviteOnly() && !channel.isInvited(fd))
	{
		server_.msgClient(fd, ERR_INVITEONLYCHAN(server_.getIP(), channelName));
		return;
	}
	if (channel.isFull())
	{
		server_.msgClient(fd, ERR_CHANNELISFULL(server_.getIP(), channelName));
		return;
	}

	channel.addMember(fd);
	std::string joinMsg = RPL_JOIN(server_.getIP(), server_.getClient(fd).getNickName(), channelName);
	for (int memberFd : channel.getMembers())
		server_.msgClient(memberFd, joinMsg);
	if (isFirstMember)
	{
		channel.addOperator(fd);
		server_.msgClient(fd, RPL_SERVERMODE(server_.getIP(), channelName, "+o", server_.getClient(fd).getNickName()));
		server_.msgClient(fd, RPL_YOUROP);
	}

	if (channel.getTopic().empty())
		server_.msgClient(fd, RPL_NOTOPIC(server_.getIP(), server_.getClient(fd).getNickName(), channelName));
	else
		server_.msgClient(fd, RPL_TOPIC(server_.getIP(), server_.getClient(fd).getNickName(), channelName, channel.getTopic()));
	Logger::info("Client " + server_.getClient(fd).getNickName() + " joined channel " + channelName);
}

void CommandHandler::cmdMsg(int fd, const std::vector<std::string>& params)
{
    if (params.size() <2)
    {
        server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "PRIVMSG"));
        Logger::warning("PRIVMSG: Not enough parameters from fd=" + std::to_string(fd));
        return;
    }
    std::string recipient = params[0];
    std::string message;

    for (size_t i = 1; i < params.size(); ++i)
    {
        message += params[i];
        if (i != params.size() - 1)
            message += " ";
    }

    if (!message.empty() && message[0] == ':')
        message = message.substr(1);
    if (message.empty())
    {
        server_.msgClient(fd, ERR_NOTEXTTOSEND(server_.getIP()));
        return;
    }

    const std::string& senderNick = server_.getClient(fd).getNickName();

    if (!recipient.empty() && recipient[0] == '#')
    {
        std::map<std::string, Channel>& channels = server_.getChannels();
        std::map<std::string, Channel>::iterator it = channels.find(recipient);
        if (it == channels.end())
        {
            server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), recipient));
            return;
        }
        Channel& channel = it->second;
        if (!channel.isMember(fd))
        {
            server_.msgClient(fd, ERR_CANNOTSENDTOCHAN(server_.getIP(), recipient));
            return;
        }
        std::string privmsg = RPL_PRIVMSG(server_.getIP(), senderNick, recipient, message);
        const std::set<int>& members = channel.getMembers();
        for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            if (*it != fd)
                server_.msgClient(*it, privmsg);
        }

        Logger::info("Message sent from " + senderNick + " to channel " + recipient);
    }
    else
    {
        int targetFd = server_.getClientFdByNickName(recipient);
        if (targetFd == -1)
        {
            server_.msgClient(fd, ERR_NOSUCHNICK(server_.getIP(), recipient));
            return;
        }
        std::string privmsg = RPL_PRIVMSGFORMAT(senderNick, recipient, message);
        server_.msgClient(targetFd, privmsg);
        Logger::log(LogLevel::Privmsg, "Message sent from " + senderNick + " to " + recipient);
    }
}

//void CommandHandler::cmdQuit(int fd, const std::vector<std::string>& params)
//{
//	Client& client = server_.getClient(fd);
//	std::string quitMsg = (params.empty()) ? "Client Quit" : params[0];
//	std::string fullQuitMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost QUIT :" + quitMsg + "\r\n";

//	std::set<int> notifiedClients;
//	for (const auto& channelPair : server_.getChannels())
//	{
//		const Channel& channel = channelPair.second;
//		if (channel.isMember(fd))
//		{
//			for (int memberFd : channel.getMembers())
//			{
//				if (notifiedClients.insert(memberFd).second && memberFd != fd)
//				{
//					server_.msgClient(memberFd, fullQuitMsg);
//				}
//			}
//			server_.removeClientFromChannel(fd, channelPair.first);
//		}
//	}
//	Logger::info("Client with fd=" + std::to_string(fd) + " quit: " + quitMsg);
//	server_.msgClient(fd, fullQuitMsg);
//	server_.removeClient(fd);
//}

void CommandHandler::cmdQuit(int fd, const std::vector<std::string>& params)
{
	// First check if client exists
	if (server_.getClients().find(fd) == server_.getClients().end()) {
		Logger::warning("QUIT: Client fd=" + std::to_string(fd) + " not found");
		return;
	}

	Client& client = server_.getClient(fd);
	std::string quitMsg = (params.empty()) ? "Client Quit" : params[0];
	std::string fullQuitMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost QUIT :" + quitMsg + "\r\n";

	// Use a copy of channels to avoid modifying while iterating
	std::map<std::string, Channel> channelsCopy = server_.getChannels();
	std::set<int> notifiedClients;

	for (const auto& channelPair : channelsCopy)
	{
		const Channel& channel = channelPair.second;
		if (channel.isMember(fd))
		{
			// Get a copy of members to avoid iteration issues
			std::set<int> members = channel.getMembers();
			for (int memberFd : members)
			{
				if (notifiedClients.insert(memberFd).second && memberFd != fd)
				{
					server_.msgClient(memberFd, fullQuitMsg);
				}
			}
			server_.removeClientFromChannel(fd, channelPair.first);
		}
	}

	Logger::info("Client with fd=" + std::to_string(fd) + " quit: " + quitMsg);
	//server_.msgClient(fd, fullQuitMsg);
	server_.removeClient(fd);
}

void CommandHandler::cmdPing(int fd, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		server_.msgClient(fd, ERR_NOORIGIN(server_.getIP()));
		return;
	}

	std::string token = params[0];
	std::string response = ":" + server_.getIP() + " PONG " + server_.getIP() + " :" + token + "\r\n";

	Logger::log(LogLevel::Ping, "Received PING from client fd=" + std::to_string(fd));
	server_.msgClient(fd, response);
}

void CommandHandler::cmdPong(int fd, const std::vector<std::string>& params)
{
	(void)params;
	std::map<int, Client>& clients = server_.getClients();

	auto it = clients.find(fd);
	if (it != clients.end())
	{
		it->second.updatePongReceived();
		Logger::log(LogLevel::Pong, "Received PONG from client fd=" + std::to_string(fd));
	}
	else
	{
		Logger::warning("Received PONG from unknown client fd=" + std::to_string(fd));
	}
}

void CommandHandler::cmdTopic(int fd, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "TOPIC"));
		Logger::warning("TOPIC: Not enough parameters from fd=" + std::to_string(fd));
		return;
	}

	const std::string& channelName = params[0];
	std::string topic;

	if (params.size() > 1)
	{
		for (size_t i = 1; i < params.size(); ++i)
		{
			topic += params[i];
			if (i != params.size() - 1)
				topic += " ";
		}
		if (!topic.empty() && topic[0] == ':')
			topic = topic.substr(1);
	}
	std::map<std::string, Channel>& channels = server_.getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end())
	{
		server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), channelName));
		return;
	}

	Channel& channel = it->second;
	if (!channel.isMember(fd))
	{
		server_.msgClient(fd, ERR_NOTONCHANNEL(server_.getIP(), channelName));
		return;
	}

	const std::string& nick = server_.getClient(fd).getNickName();

	if (topic.empty())
	{
		const std::string& currentTopic = channel.getTopic();
		if (currentTopic.empty())
			server_.msgClient(fd, RPL_NOTOPIC(server_.getIP(), nick, channelName));
		else
			server_.msgClient(fd, RPL_TOPIC(server_.getIP(), nick, channelName, currentTopic));
	}

	if (channel.isTopicRestriction() && !channel.isOperator(fd))
	{
		server_.msgClient(fd, ERR_CHANOPRIVISNEEDED(server_.getIP(), channelName));
		return;
	}

	channel.setTopic(topic);
	std::string topicMsg = RPL_TOPIC(server_.getIP(), nick, channelName, topic);

	const std::set<int>& members = channel.getMembers();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		server_.msgClient(*it, topicMsg);
	}
	Logger::info("Topic set to '" + topic + "' for channel " + channelName + " by " + nick);
}

void CommandHandler::cmdKick(int fd, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "KICK"));
		Logger::warning("KICK: Not enough parameters from fd=" + std::to_string(fd));
		return;
	}

	std::string channelName = params[0];
	std::string targetNick = params[1];
	std::string kickReason;

	if (channelName[0] != '#')
		channelName = "#" + channelName;

	if (params.size() > 2)
	{
		for (size_t i = 2; i < params.size(); ++i)
		{
			kickReason += params[i];
			if (i != params.size() - 1)
				kickReason += " ";
		}
		if (!kickReason.empty() && kickReason[0] == ':')
			kickReason = kickReason.substr(1);
	}

	if (kickReason.empty())
		kickReason = "No reason";

	std::map<std::string, Channel> &channels = server_.getChannels();
	auto channelIt = channels.find(channelName);
	if (channelIt == channels.end())
	{
		server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), channelName));
		return;
	}

	Channel &channel = channelIt->second;
	if (!channel.isMember(fd))
	{
		server_.msgClient(fd, ERR_NOTONCHANNEL(server_.getIP(), channelName));
		return;
	}

	if (!channel.isOperator(fd))
	{
		server_.msgClient(fd, ERR_CHANOPRIVISNEEDED(server_.getIP(), channelName));
		return;
	}

	int targetFd = -1;
	for (const auto& pair : server_.getClients())
	{
		if (pair.second.getNickName() == targetNick)
		{
			targetFd = pair.first;
			break;
		}
	}

	if (targetFd == -1)
	{
		server_.msgClient(fd, ERR_NOSUCHNICK(server_.getIP(), targetNick));
		return;
	}

	if (!channel.isMember(targetFd))
	{
		server_.msgClient(fd, ERR_USERNOTINCHANNEL(server_.getIP(), targetNick, channelName));
		return;
	}

	const std::string &kickerNick = server_.getClient(fd).getNickName();
	std::string kickMsg = ":" + kickerNick + "!" + server_.getClient(fd).getUserName() +
						"@" + server_.getIP() + " KICK " + channelName + " " +
						targetNick + " :" + kickReason;

	for (int memberFd : channel.getMembers())
	{
		server_.msgClient(memberFd, kickMsg);
	}

	server_.removeClientFromChannel(targetFd, channelName);

	Logger::log(LogLevel::Channel, "User " + targetNick + " was kicked from " +
			channelName + " by " + kickerNick + " (" + kickReason + ")");
}

void CommandHandler::cmdInvite(int fd, const std::vector<std::string>& params)
{
    if (params.size() < 2)
    {
        server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "INVITE"));
        Logger::warning("INVITE: Not enough parameters from fd=" + std::to_string(fd));
        return;
    }

    const std::string& recipientNick = params[0];
    const std::string& channelName = params[1];

    std::map<std::string, Channel>& channels = server_.getChannels();
    if (channels.find(channelName) == channels.end())
    {
        server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), channelName));
        return;
    }

    Channel& channel = channels[channelName];
    if (!channel.isMember(fd))
    {
        server_.msgClient(fd, ERR_NOTONCHANNEL(server_.getIP(), channelName));
        return;
    }

    int targetFd = server_.getClientFdByNickName(recipientNick);
    if (targetFd == -1)
    {
        server_.msgClient(fd, ERR_NOSUCHNICK(server_.getIP(), recipientNick));
        return;
    }

    channel.invite(targetFd);

    const Client& inviter = server_.getClient(fd);

    std::string userhost = RPL_USERHOST(inviter.getNickName(), inviter.getUserName(), server_.getIP());

    server_.msgClient(targetFd, RPL_INVITE(server_.getIP(), inviter.getNickName(), recipientNick, channelName));

    server_.msgClient(fd, RPL_INVITEFORMAT(userhost, recipientNick, channelName));

    Logger::info("INVITE: " + inviter.getNickName() + " invited " + recipientNick + " to " + channelName);
}

void CommandHandler::cmdMode(int fd, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "MODE"));
		Logger::warning("MODE: Not enough parameters from fd=" + std::to_string(fd));
		return;
	}

	std::string channelName = params[0];
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	std::map<std::string, Channel>& channels = server_.getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end()) {
		server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), channelName));
		return;
	}

	Channel& channel = it->second;
	if (!channel.isMember(fd)) {
		server_.msgClient(fd, ERR_NOTONCHANNEL(server_.getIP(), channelName));
		return;
	}

	if (params.size() < 2) {

	std::string modes;
	if (channel.isInviteOnly()) modes += "i";
	if (channel.isTopicRestriction()) modes += "t";
	if (channel.hasPassword()) modes += "k";
	if (channel.hasUserLimit()) modes += "l";

	std::string reply = ":" + server_.getIP() + " 324 " + server_.getClient(fd).getNickName()
						  + " " + channelName + " +" + modes + "\r\n";
	server_.msgClient(fd, reply);
	return;
	}
	const std::string& modeStr = params[1];
	bool adding = true;
	size_t paramIndex = 2;

	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];
		if (c == '+') { adding = true; continue; }
		if (c == '-') { adding = false; continue; }

		switch (c)
		{
			case 'i':
				channel.setInviteOnly(adding);
				break;
			case 't':
				channel.setTopicRestriction(adding); // To Do in Channel.hpp
				break;
			case 'k':
				if (adding) {
					if (paramIndex >= params.size()) {
						server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "MODE +k"));
						return;
					}
					channel.setPassword(params[paramIndex++]);
				} else {
					channel.setPassword("");
				}
				break;
			case 'l':
				if (adding) {
					if (paramIndex >= params.size()) {
						server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "MODE +l"));
						return;
					}
					channel.setUserLimit(std::atoi(params[paramIndex++].c_str()));
				} else {
					channel.setUserLimit(-1); // no limit
				}
				break;
			default:
				server_.msgClient(fd, ERR_UNKNOWNMODE(server_.getIP(), std::string(1, c), channelName));
				Logger::warning("MODE: Unknown mode '" + std::string(1, c) + "' from fd=" + std::to_string(fd));
				return;
		}
	}

	Logger::info("Client " + server_.getClient(fd).getNickName() + " changed mode of " + channelName + " to " + modeStr);
}

void CommandHandler::cmdPart(int fd, const std::vector<std::string>& params)
{
	if (params.empty())
	{
		server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getIP(), "PART"));
		Logger::warning("PART: Not enough parameters from fd=" + std::to_string(fd));
		return;
	}

	std::string channelName = params[0];
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	std::map<std::string, Channel>& channels = server_.getChannels();
	std::map<std::string, Channel>::iterator it = channels.find(channelName);

	if (it == channels.end())
	{
		server_.msgClient(fd, ERR_NOSUCHCHANNEL(server_.getIP(), channelName));
		return;
	}

	Channel& channel = it->second;
	if (!channel.isMember(fd))
	{
		server_.msgClient(fd, ERR_NOTONCHANNEL(server_.getIP(), channelName));
		return;
	}

	// Optional part message
	std::string partMsg = (params.size() > 1) ? params[1] : "";
	for (size_t i = 2; i < params.size(); ++i)
	{
	   partMsg += " " + params[i];
	}
	if (!partMsg.empty() && partMsg[0] == ':')
		partMsg = partMsg.substr(1);

	const Client& client = server_.getClient(fd);
	std::string fullMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@localhost PART " + channelName;
	if (!partMsg.empty())
		fullMsg += " :" + partMsg;
	fullMsg += "\r\n";

	// Notify all members
	const std::set<int>& members = channel.getMembers();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		server_.msgClient(*it, fullMsg);
	}

	Logger::info("Client " + client.getNickName() + " parted from " + channelName + (partMsg.empty() ? "" : " (" + partMsg + ")"));

	// Remove user from channel
	server_.removeClientFromChannel(fd, channelName);
}
