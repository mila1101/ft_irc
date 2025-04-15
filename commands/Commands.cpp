#include "Server.hpp"

void Server::addCommand(const std::string& name, cmdFunction handler)
{
    commandTable[name] = std::bind(handler, this, std::placeholders::_1, std::placeholders::_2);;
}

void Server::commandInit()
{
    addCommand("NICK", &Server::cmdNick);
    addCommand("USER", &Server::cmdUser);
    addCommand("JOIN", &Server::cmdJoin);
    addCommand("PRIVMSG", &Server::cmdMsg);
    addCommand("QUIT", &Server::cmdQuit);
    addCommand("PING", &Server::cmdPing);
    addCommand("PONG", &Server::cmdPong);
    addCommand("TOPIC", &Server::cmdTopic);
    addCommand("KICK", &Server::cmdKick);
    addCommand("INVITE", &Server::cmdInvite);
    addCommand("MODE", &Server::cmdMode);
    addCommand("PART", &Server::cmdPart);
}