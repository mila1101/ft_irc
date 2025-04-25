/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:10 by eahn              #+#    #+#             */
/*   Updated: 2025/04/25 16:01:59 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "SocketHandler.hpp"
#include "../utils/Logger.hpp"
#include "../commands/CommandHandler.hpp"

Server* Server::instance_ = nullptr;  // init static singleton pointer

Server::Server (int port, const std::string& password)
	: port_(port), password_(password), listenFd_(-1), running_(false)
{
	instance_ = this;

	setupSignalHandler();

	if (!initSocket())
		throw std::runtime_error("Failed to initialize server socket");

	char hostBuffer[256];
	if (gethostname(hostBuffer, sizeof(hostBuffer)) == 0) {
		serverIp_ = std::string(hostBuffer);
	} else {
		serverIp_ = "127.0.0.1";
	}

	serverName_ = serverIp_;

	setupPoll();

	commandHandler_ = std::make_unique<CommandHandler>(*this);
    socketHandler_ = std::make_unique<SocketHandler>(pollFds_, *commandHandler_, *this);

	Logger::info ("Server initialized on port " + std::to_string(port_));
}

Server::~Server()
{
	if (listenFd_ != -1)
		close(listenFd_);
}

void Server::setupSignalHandler()
{
	struct sigaction sa;
	sa.sa_handler = Server::handleSignal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, nullptr); // Handle Ctrl+C
}

void Server::handleSignal(int signal)
{
	if (signal == SIGINT)
	{
		Logger::info("Received SIGINT, stopping server...");
		if (instance_)
			instance_->stop();
	}
}

void Server::run()
{
	running_ = true;
	Logger::info ("Server is running");

	while (running_)
	{
		pollLoop();
	}

	Logger::info("Server stopped");
}

void Server::stop()
{
	running_ = false;

	Logger::info("Cleaning up clients...");
    for (size_t i = 0; i < pollFds_.size(); ++i)
    {
        int fd = pollFds_[i].fd;
        if (fd != listenFd_)
            close(fd);
    }

    pollFds_.clear();

    Logger::info("All client connections closed.");
}

bool Server::initSocket()
{
	// 1. Create TCP socket
	listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFd_ < 0)
	{
		Logger::error("Failed to create socket", true);
		return false;
	}

	// 2. Allow address reuse (avoid "Address already in use" error)
	int opt = 1;
	if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		Logger::error("Failed to set socket options", true);
		return false;
	}

	// 3. Set socket to non-blocking mode
	if (fcntl(listenFd_, F_SETFL, O_NONBLOCK) < 0)
	{
		Logger::error ("Failed to set socket to non-blocking", true);
		return false;
	}

	// 4. Bind to the specified IP/port
	serverAddr_.sin_family = AF_INET;
	serverAddr_.sin_addr.s_addr = INADDR_ANY;
	serverAddr_.sin_port = htons(port_);

	if (bind(listenFd_, (struct sockaddr*)&serverAddr_, sizeof(serverAddr_)) < 0)
	{
		Logger::error("Failed to bind socket", true);
		return false;
	}

	// 5. Begin listening for incoming connections
	if (listen(listenFd_, SOMAXCONN) < 0)
	{
		Logger::error ("Failed to listen on socket", true);
		return false;
	}

	return true;
}

void Server::setupPoll()
{
	struct pollfd listenPollFd =
	{
		.fd = listenFd_,
		.events = POLLIN,
		.revents = 0
	};
	pollFds_.push_back(listenPollFd);
}

void Server::pollLoop()
{
	int ready = poll(pollFds_.data(), pollFds_.size(), 1000); // changed from -1 to 1000, pingpong works but there is segfault
	if (ready < 0)
	{
		Logger::error ("Poll error");
		return ;
	}

	for (size_t i = 0; i < pollFds_.size(); ++i)
	{
		int fd = pollFds_[i].fd;

		if (pollFds_[i].revents & POLLIN)
		{
			if (fd == listenFd_)
			{
				handleIncomingConnection();
			}
			else
			{
				handleClientMessage(fd);
			}
		}
	}
	checkClientHeartbeats();
}

void Server::handleIncomingConnection()
{
	socketHandler_->acceptConnection(listenFd_);
}

void Server::handleClientMessage(int clientFd)
{
	socketHandler_->receiveMessage(clientFd);
}

void Server::msgClient(int clientSocket, const std::string& msg)
{
	std::string endMsg = msg + "\r\n";
	ssize_t bytes = send(clientSocket, endMsg.c_str(), endMsg.length(), 0);

	if (bytes == -1)
	{
		std::cerr << "Failed to send message to client " \
		<< clientSocket << ": " << strerror(errno) << std::endl;
	}
}

std::string Server::getIP() const
{
	return serverIp_;
}

const std::string& Server::getPassword() const {
    return password_;
}

std::string Server::getServerName() const {
    return serverName_;
}

Client& Server::getClient(int fd)
{
	return clients_.at(fd);
}

std::map<int, Client>& Server::getClients()
{
	return clients_;
}

Channel& Server::getChannel(const std::string& name)
{
	return channels_.at(name);
}
std::map<std::string, Channel>& Server::getChannels()
{
	return channels_;
}

Channel& Server::getOrCreateChannel(const std::string& name)
{
	if (channels_.find(name) == channels_.end())
	{
		channels_[name] = Channel(name);
	}
	return channels_.at(name);
}

void Server::removeClient(int fd) {
	// First, check if client exists
	auto clientIt = clients_.find(fd);
	if (clientIt == clients_.end()) {
		Logger::warning("Attempted to remove non-existent client fd=" + std::to_string(fd));
		return;
	}

	Client& client = clientIt->second;
	std::string nickname = client.getNickName();

	// Remove from all channels
	std::map<std::string, Channel> channelsCopy = channels_; // Make copy to avoid iterator invalidation
	for (const auto& channelPair : channelsCopy) {
		if (channelPair.second.isMember(fd)) {
			removeClientFromChannel(fd, channelPair.first);
		}
	}

	// Now remove from clients map
	clients_.erase(fd);

	// Close socket (should trigger SocketHandler::disconnectClient)
	close(fd);

	Logger::info("Client " + nickname + " (fd=" + std::to_string(fd) + ") fully removed from server");
}

void Server::removeClientFromChannel(int fd, const std::string& channelName)
{
	std::map<std::string, Channel>::iterator it = channels_.find(channelName);
	if (it != channels_.end())
	{
		// First check if client is in the channel
		if (it->second.isMember(fd))
		{
			// Remove client from the channel
			it->second.removeMember(fd);

			// Check if channel is now empty and should be removed
			if (it->second.isEmpty())
			{
				Logger::info("Channel " + channelName + " is now empty, removing it");
				channels_.erase(it);
			}
		}
	}
}

void Server::sendWelcome(int fd, const Client& client)
{
	std::string nick = client.getNickName();
	std::string serverName = getServerName();

	std::string welcome = ":" + serverName + " 001 " + nick + " :" + COLOR_GREEN + " Welcome to the IRC server!\r" + COLOR_RESET;
	std::string yourHost = ":" + serverName + " 002 " + nick + " : Your host is " + serverName + ", runnig version 1.0\r";
	std::string created = ":" + serverName + " 003 " + nick + " : This server was created just now\r";
	std::string myInfo = ":" + serverName + " 004 " + nick + " " + serverName + " v1.0 iotl\r";

	msgClient(fd, welcome);
	msgClient(fd, yourHost);
	msgClient(fd, created);
	msgClient(fd, myInfo);

	Logger::info("Welcome sequence sent to " + nick);
}


int Server::getClientFdByNickName(const std::string& nick) const
{
    for (std::map<int, Client>::const_iterator it = clients_.begin(); it != clients_.end(); ++it)
    {
        if (it->second.getNickName() == nick)
            return it->first;  // return fd
    }
    return -1;  // not found
}

void Server::checkClientHeartbeats() {
	std::vector<int> toRemove;

	for (std::map<int, Client>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
		int fd = it->first;
		Client& client = it->second;

		if (client.needsPing()) {
			std::string pingMsg = "PING :" + serverName_;
			msgClient(fd, pingMsg);
			client.setPingSent();
			Logger::log(LogLevel::Ping, "Sent PING to client fd=" + std::to_string(fd));
		}
		else if (client.hasTimedOut()) {
			Logger::warning("Client fd=" + std::to_string(fd) + " timed out (no PONG received)");
			toRemove.push_back(fd);
		}
	}

	for (size_t i = 0; i < toRemove.size(); ++i) {
		removeClient(toRemove[i]);
	}
}
