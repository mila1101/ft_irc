/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:03:14 by eahn              #+#    #+#             */
/*   Updated: 2025/04/15 16:02:04 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketHandler.hpp"

SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

// To Do
void SocketHandler::acceptConnection(int listenFd, std::vector<struct pollfd>& pollFds)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int clientFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientFd < 0)
	{
		Logger::log(LogLevel::Error, "Failed to accept connection");
		return;
	}

	Logger::log(LogLevel::Connection, "New connection from " + std::string(inet_ntoa(clientAddr.sin_addr)));

	addClientSocket(clientFd, pollFds);
}

// To Do
void SocketHandler::receiveMessage(int clientFd)
{
	char buffer[1024];

	for (size_t i = 0; i < sizeof(buffer); ++i)
		buffer[i] = 0;

	ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
		{
			Logger::log(LogLevel::Disconnection, "Client disconnected");
		}
		else
		{
			Logger::log(LogLevel::Error, "Error reading from client");
		}

		if (pollFds_)
			disconnectClient(clientFd, *pollFds_);
		else
			Logger::log(LogLevel::Error, "pollFds_ is NULL");

		return;
	}

	Logger::log(LogLevel::Privmsg, "Received: " + std::string(buffer));

	send(clientFd, buffer, bytesRead, 0);
}

// To Do
void SocketHandler::addClientSocket(int clientFd, std::vector<struct pollfd>& pollFds)
{
	struct pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN;
	clientPollFd.revents = 0;

	pollFds.push_back(clientPollFd);
}

// To Do
void SocketHandler::disconnectClient(int clientFd, std::vector<struct pollfd>& pollFds)
{
	close(clientFd);

	for (std::vector<struct pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); )
	{
		if (it->fd == clientFd)
			it = pollFds.erase(it);
		else
			++it;
	}

	Logger::log(LogLevel::Disconnection, "Client disconnected");
}
