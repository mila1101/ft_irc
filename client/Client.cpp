/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 17:07:35 by msoklova          #+#    #+#             */
/*   Updated: 2025/04/17 17:07:35 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "Client.hpp"

Client::Client()
	: socket_(-1),
	  ipAddress_(""),
	  nickName_(""),
	  userName_(""),
	  realName_(""),
	  passwordVerified_(false),
	  registered_(false),
	  receiveBuffer_(""),
	  awaitingPong_(false)
{
	lastPing_ = std::chrono::system_clock::now();
	lastPong_ = std::chrono::system_clock::now();
}

Client::Client(const std::string &ipAddress, int socket)
	: socket_(socket),
	  ipAddress_(ipAddress),
	  nickName_(""),
	  userName_(""),
	  realName_(""),
	  passwordVerified_(false),
	  registered_(false),
	  receiveBuffer_(""),
	  awaitingPong_(false)
{
	lastPing_ = std::chrono::system_clock::now();
	lastPong_ = std::chrono::system_clock::now();
}

Client::~Client()
{
}

bool Client::operator==(const Client &other) const
{
	return socket_ == other.socket_;
}

bool Client::isInChannel(const std::string &channel) const
{
	return joinedChannels_.find(channel) != joinedChannels_.end();
}

void Client::addChannel(const std::string &channel)
{
	joinedChannels_.insert(channel);
}

void Client::removeChannel(const std::string &channel)
{
	joinedChannels_.erase(channel);
}

void Client::appendToBuffer(const std::string &data)
{
	receiveBuffer_ += data;
}

std::string Client::getBuffer() const
{
	return receiveBuffer_;
}

void Client::clearBuffer()
{
	receiveBuffer_.clear();
}

void Client::updatePongReceived()
{
	lastPong_ = std::chrono::system_clock::now();
	awaitingPong_ = false;
}

void Client::setPingSent()
{
	lastPing_ = std::chrono::system_clock::now();
	awaitingPong_ = true;
}

bool Client::needsPing() const
{
	if (awaitingPong_) {
		return false;
	}
	
	auto now = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPong_).count();
	return duration >= 120; 
}

bool Client::hasTimedOut() const
{
	if (!awaitingPong_) {
		return false;
	}
	
	auto now = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPing_).count();
	return duration >= 60; 
}
