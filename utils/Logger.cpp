/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:30:17 by eahn              #+#    #+#             */
/*   Updated: 2025/04/15 15:44:26 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define GRAY    "\033[90m"

void Logger::logMessage (LogLevel level, const std::string& message, bool exitAfter)
{
	std::time_t now = std::time(nullptr);
	std::tm* tm_info = std::localtime(&now);
	std::ostringstream timeStream;
	timeStream << std::put_time(tm_info, "%H:%M:%S");

	std::string levelStr;
	std::string color;

	switch(level)
	{
		case LogLevel::Info: levelStr = "INFO"; color = BLUE; break;
		case LogLevel::Warning: levelStr = "WARN"; color = YELLOW; break;
		case LogLevel::Error: levelStr = "ERROR"; color = RED; break;
		case LogLevel::Connection: levelStr = "CONNECT"; color = GREEN; break;
		case LogLevel::Disconnection: levelStr = "DISCONNECT"; color = RED; break;
		case LogLevel::Ping: levelStr = "PING"; color = CYAN; break;
		case LogLevel::Pong: levelStr = "PONG"; color = CYAN; break;
		case LogLevel::Channel: levelStr = "CHANNEL"; color = MAGENTA; break;
		case LogLevel::Privmsg: levelStr = "PRIVMSG"; color = GRAY; break;
	}

	std::cout << color
			<< levelStr << ": " << message
			<< " @ " << timeStream.str()
			<< RESET << std::endl;

	if (exitAfter)
		std::exit(1);
}

void Logger::info (const std::string& message)
{
	logMessage(LogLevel::Info, message);
}

void Logger::warning (const std::string& message)
{
	logMessage(LogLevel::Warning, message);
}

void Logger::error (const std::string& message, bool exitAfter)
{
	logMessage(LogLevel::Error, message, exitAfter);
}
