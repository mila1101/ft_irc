/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:30:17 by eahn              #+#    #+#             */
/*   Updated: 2025/04/13 23:36:03 by eahn             ###   ########.fr       */
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
}
