/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smiranda <smiranda@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:33:36 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 15:03:10 by smiranda         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../utils/Logger.hpp"
#include <iostream>
#include <sys/socket.h> // for send()

CommandHandler::CommandHandler()
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
//     // clientManager.setNickname(fd, newNick);

//     Logger::info("NICK command accepted: " + newNick + " (fd=" + std::to_string(fd) + ")");

//     std::string msg = ":" + newNick + " NICK " + newNick + "\r\n";
//     send(fd, msg.c_str(), msg.length(), 0);  // Send success response
// }

void CommandHandler::cmdNick(int fd, const std::vector<std::string>& params)
{
    // Check if parameter is empty
    if (params.empty())
    {
        Logger::warning("NICK: no nickname provided from fd=" + std::to_string(fd));
        std::string err = "431 :No nickname given\r\n";
        send(fd, err.c_str(), err.length(), 0);
        return;
    }
    std::regex pattern("^[a-zA-Z][a-zA-Z0-9\\-_]{0,8}$");
    std::string newNick = params[0];
    if (!std::regex_match(newNick, pattern))
    {
        
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


// To do
void CommandHandler::cmdUser(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdJoin(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdMsg(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdQuit(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdPing(int fd, const std::vector<std::string>& params)
{

}

void CommandHandler::cmdPong(int fd, const std::vector<std::string>& params)
{

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