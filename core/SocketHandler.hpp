/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:03:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/13 23:13:25 by eahn             ###   ########.fr       */
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

class SocketHandler
{
	public:
		SocketHandler();
		~SocketHandler();

		void acceptConnection(int listenFd, std::vector<struct pollfd>& pollFds);
		void receiveMessage(int clientFd);

	private:
		void addClientSocket(int clientFd, std::vector<struct pollfd>& pollFds);
		void disconnectClient(int clientFd, std::vector<struct pollfd>& pollFds);
};
