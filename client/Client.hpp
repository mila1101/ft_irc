/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 17:07:50 by msoklova          #+#    #+#             */
/*   Updated: 2025/04/22 17:01:12 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <set>
#include <chrono>
#include "../utils/Macros.hpp"

class Client
{
	private:
		int socket_;
		std::string ipAddress_;
		std::string nickName_;
		std::string userName_;
		std::string realName_;
		bool passwordVerified_;
		bool registered_;
		std::string receiveBuffer_;
		std::set<std::string> joinedChannels_;
		std::chrono::system_clock::time_point lastPing_;
		std::chrono::system_clock::time_point lastPong_;
		bool awaitingPong_;

	public:
		Client();
		Client(const std::string &ipAddress, int socket);
		bool operator==(const Client &other) const;
		~Client();

		int getSocket() const { return socket_; }
		const std::string &getIpAddress() const { return ipAddress_; }
		
		const std::string &getNickName() const { return nickName_; }
		void setNickName(const std::string &nickName) { nickName_ = nickName; }
		
		const std::string &getUserName() const { return userName_; }
		void setUserName(const std::string &userName) { userName_ = userName; }
		
		const std::string &getRealName() const { return realName_; }
		void setRealName(const std::string &realName) { realName_ = realName; }
		
		bool isPasswordVerified() const { return passwordVerified_; }
		void setPasswordVerified(bool verified) { passwordVerified_ = verified; }
		
		bool isRegistered() const { return registered_; }
		void setRegistered(bool registered) { registered_ = registered; }
		
		bool isInChannel(const std::string &channel) const;
		void addChannel(const std::string &channel);
		void removeChannel(const std::string &channel);
		
		void appendToBuffer(const std::string &data);
		std::string getBuffer() const;
		void clearBuffer();
		
		void updatePongReceived();
		void setPingSent();
		bool needsPing() const;
		bool hasTimedOut() const;
};