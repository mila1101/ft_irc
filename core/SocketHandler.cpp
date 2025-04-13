/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:03:14 by eahn              #+#    #+#             */
/*   Updated: 2025/04/13 23:16:09 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketHandler.hpp"
#include "utils/Logger.hpp"

SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

// To Do
void SocketHandler::acceptConnection(int listenFd, std::vector<struct pollfd>& pollFds)
{

}

// To Do
void SocketHandler::receiveMessage(int clientFd)
{

}

// To Do
void SocketHandler::addClientSocket(int clientFd, std::vector<struct pollfd>& pollFds)
{
}

// To Do
void SocketHandler::disconnectClient(int clientFd, std::vector<struct pollfd>& pollFds)
{
}
