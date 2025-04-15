/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:30:17 by eahn              #+#    #+#             */
/*   Updated: 2025/04/15 15:29:15 by msoklova         ###   ########.fr       */
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

void Logger::log (LogLevel level, const std::string& message, bool exitAfter)
{
	std::time_t now = std::time(nullptr);
	std::tm* tm_info = std::localtime(&now);
	std::ostringstream timeStream;
	timeStream << std::put_time(tm_info, "%H:%M:%S");

	std::string levelStr;
	std::string color;

	switch (level)
	{
		case LogLevel::Info:
			levelStr = "[INFO]";
			color = GREEN;
			break;
		case LogLevel::Error:
			levelStr = "[ERROR]";
			color = RED;
			break;
		case LogLevel::Warning:
			levelStr = "[WARNING]";
			color = YELLOW;
			break;
		case LogLevel::Connection:
			levelStr = "[CONNECTION]";
			color = CYAN;
			break;
		case LogLevel::Disconnection:
			levelStr = "[DISCONNECTION]";
			color = MAGENTA;
			break;
		case LogLevel::Privmsg:
			levelStr = "[MESSAGE]";
			color = BLUE;
			break;
		default:
			levelStr = "[UNKNOWN]";
			color = GRAY;
			break;
	}

	std::cout << color << timeStream.str() << " " << levelStr << " " << message << RESET << std::endl;

	if (exitAfter)
		exit(1);
}
