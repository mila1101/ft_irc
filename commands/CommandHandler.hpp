/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:33:34 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 01:15:06 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional> 
#include "../utils/InputParser.hpp" // for ParsedCommand

class CommandHandler
{
	public:
  		CommandHandler();
    	~CommandHandler();

		void dispatch(int clientFd, const ParsedCommand& command);

	private:
		// command string -> handler function
    	typedef void (CommandHandler::*CommandFunc)(int, const std::vector<std::string>&);
    	std::map<std::string, std::function<void(int, const std::vector<std::string>&)> > commandTable;

		// add command
		 void addCommand(const std::string& name, CommandFunc handler);

		// command implementation (To do by Siria)
		void cmdNick(int clientFd, const std::vector<std::string>& params);
		void cmdUser(int clientFd, const std::vector<std::string>& params);
		void cmdJoin(int clientFd, const std::vector<std::string>& params);
		void cmdMsg(int clientFd, const std::vector<std::string>& params);
		void cmdQuit(int clientFd, const std::vector<std::string>& params);
		void cmdPing(int clientFd, const std::vector<std::string>& params);
		void cmdPong(int clientFd, const std::vector<std::string>& params);
		void cmdTopic(int clientFd, const std::vector<std::string>& params);
		void cmdKick(int clientFd, const std::vector<std::string>& params);
		void cmdInvite(int clientFd, const std::vector<std::string>& params);
		void cmdMode(int clientFd, const std::vector<std::string>& params);
		void cmdPart(int clientFd, const std::vector<std::string>& params);
		
};
