/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smiranda <smiranda@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:33:36 by eahn              #+#    #+#             */
/*   Updated: 2025/04/21 16:55:33 by smiranda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../utils/Logger.hpp"
#include "../client/Channel.hpp"
#include "../client/Client.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server &server, std::map<int, Client>& clients) : server_(server), clients_(clients)
{
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

// To do by Siria : implement all command handlers

// Example: NICK command handler
// void CommandHandler::cmdNick(int fd, const std::vector<std::string>& params)
// {
//     // Check if nickname was provided
//     if (params.empty()) {
//         Logger::warning("NICK: no nickname provided from fd=" + std::to_string(fd));
//         std::string err = "431 :No nickname given\r\n";
//         send(fd, err.c_str(), err.length(), 0);  // Send error message to client
//         return;
//     }

//     std::string newNick = params[0];

//     // TODO: Check if nickname is already in use (via ClientManager)
//     // if (clientManager.nickExists(newNick)) {
//     //     std::string err = "433 " + newNick + " :Nickname is already in use\r\n";
//     //     send(fd, err.c_str(), err.length(), 0);
//     //     return;
//     // }

//     // TODO: Set nickname in ClientManager
//     // clientManager.setNickName(fd, newNick);

//     Logger::info("NICK command accepted: " + newNick + " (fd=" + std::to_string(fd) + ")");

//     std::string msg = ":" + newNick + " NICK " + newNick + "\r\n";
//     send(fd, msg.c_str(), msg.length(), 0);  // Send success response
// }


// To do:
// getClients()
// getChannels()
// getIP()
// getNickName()
// setNickName()
// getMembers()
// isMember()
// isLoggedIn()
void CommandHandler::cmdNick(int clientFd, const std::vector<std::string>& params)
{
    std::map<int, Client>& clients = server_.getClients(); //getClients tbd
    const std::map<std::string, Channel>& channels = server_.getChannels(); //getChannels tbd

    if (params.empty())
    {
        Logger::warning("NICK: no nickname provided from fd=" + std::to_string(clientFd));
        server_.msgClient(clientFd, ERR_NONICKNAMEGIVEN(server_.getIP())); //getIP tbd
        return;
    }
    std::string newNick = params[0];
    std::regex pattern("^[a-zA-Z][a-zA-Z0-9\\-_]{0,8}$");

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
        if (client.isLoggedIn())
            userMsg = ":" + oldNick + "!" + client.getUserName() + "@" + server_.getIP();
        else
            userMsg = ":" + server_.getIP() + " NICK :" + newNick + "\r\n";

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

/**
 * ✨ Tips for Siria
All handlers follow the same pattern:
	- Validate params
	- Possibly interact with ClientManager or ChannelManager
	- Respond with send(fd, message...)
All IRC responses must end with \r\n (per IRC protocol)
Use Logger::log(...) for colored, categorized logs (Info, Warning, Error, etc.)
 */


// To do getters and setters
// getClients()
// getNickName()
// setUserName()
// setRealName()
// setLoggedIn()
// sendWelcome()
void CommandHandler::cmdUser(int fd, const std::vector<std::string>& params)
{
    std::map<int, Client>& clients = server_.getClients(); //tbd

    if (clients.find(clientFd) == clients.end())
        return;

    Client& client = clients[clientFd];
    if (client.isLoggedIn()) //tbd
    {
        server_.msgClient(clientFd, ERR_ALREADYREGISTRED(client.getNickName()));
        return;
    }

    if (params.size() < 4)
    {
        server_.msgClient(clientFd, ERR_NEEDMOREPARAMS(client.getNickName(), "USER"));
        return;
    }
    std::string username = params[0];
    std::string realname = params[3];
    if (!realname.empty() && realname[0] == ':')
        realname = realname.substr(1);
    client.setUsername(username);
    client.setRealname(realname);

    if (!client.getNickname().empty())
    {
        client.setLoggedIn(true);
        server_.sendWelcome(clientFd, client); //tbd
    }
}

void CommandHandler::cmdJoin(int fd, const std::vector<std::string>& params)
{
    if (params.empty())
    {
        server_.msgClient(fd, ERR_NEEDMOREPARAMS(server_.getServerName(), "JOIN"));
        return;
    }
    const std::string& channelName = params[0];
    
    if (channelName.empty() || channelName[0] != '#')
    {
        server_.msgClient(fd, ERR_NOSUCHCHANEL(server_.getServerName(), channelName));
        return;
    }

    Client& client = server_.getClient(fd);
    Channel& channel = server_.getOrCreateChannel(channelName);
    if (!channel.isMember(fd))
        channel.addMember(fd);
    
    std::string joinMsg = ":" + client.getNickName() + "!" + client.
}

void CommandHandler::cmdMsg(int fd, const std::vector<std::string>& params)
{

}

// To do:
// getClient()
// getNickName()
// getUserName()
// getChannels()
// isMember()
// getMembers()
// removeClientFromChannel() --> Remove user from the channel
// removeClient() --> Close socket and cleanup

void CommandHandler::cmdQuit(int fd, const std::vector<std::string>& params)
{
    Client& client = server_.getClient(fd);
    std::string quitMsg = (params.empty()) ? "Client Quit" : params[0];
    std::string fullQuitMsg = ":" + client.getNickName() + "!" + client.getUsername() + "@localhost QUIT :" + quitMsg + "\r\n";

    std::set<int> notifiedClients;
    for (const auto& channelPair : server_.getChannels())
    {
        const Channel& channel = channelPair.second;
        if (channel.isMember(fd))
        {
            for (int memberFd : channel.getMembers())
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
    server_.msgClient(fd, fullQuitMsg);
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

}

void CommandHandler::cmdKick(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdInvite(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdMode(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdPart(int fd, const std::vector<std::string>& params)
{

}