/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:03:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 01:10:06 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h>
#include <fcntl.h>
#include "../utils/Logger.hpp"
#include "../commands/CommandHandler.hpp"
#include <arpa/inet.h>

class SocketHandler
{
	public:
		explicit SocketHandler(std::vector<struct pollfd>& pollFds, CommandHandler& dispatcher);
		~SocketHandler();

		void acceptConnection(int listenFd);
		void receiveMessage(int clientFd);

	private:
		void addClientSocket(int clientFd);
		void disconnectClient(int clientFd);

		std::vector<struct pollfd>& pollFds_; // Eunsu: better with reference that pointer
		CommandHandler& commandHandler_;
};
