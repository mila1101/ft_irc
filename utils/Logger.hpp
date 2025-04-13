/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 23:16:55 by eahn              #+#    #+#             */
/*   Updated: 2025/04/13 23:35:22 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sstream> // ostringstream
#include <iomanip> // put_time
#include <ctime>

// Enum for log levels
enum class LogLevel {
    Info,
    Warning,
    Error,
    Connection,
    Disconnection,
    Ping,
    Pong,
    Channel,
    Privmsg
};

class Logger
{
	public:
		static void log (LogLevel level, const std::string& message, bool exitAfter = false);

		static void info (const std::string& message);
		static void warning (const std::string& message);
		static void error (const std::string& message, bool exitAfter = false);
};
