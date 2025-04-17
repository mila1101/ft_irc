/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smiranda <smiranda@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:10 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 13:42:19 by smiranda         ###   ########.fr       */
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

	setupPoll();

	commandHandler_ = std::make_unique<CommandHandler>();
    socketHandler_ = std::make_unique<SocketHandler>(pollFds_, *commandHandler_);

	socketHandler_ = std::make_unique<SocketHandler>(pollFds_);
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
	int ready = poll(pollFds_.data(), pollFds_.size(), -1);
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