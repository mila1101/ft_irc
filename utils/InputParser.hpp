/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:11:05 by eahn              #+#    #+#             */
/*   Updated: 2025/04/22 17:01:33 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This one is to be used for Command Dispatcher (Siria?)

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

// To store parsed command and its parameters
struct ParsedCommand
{
	std::string command; // Commands (e.g. "JOIN", "PRIVMSG")
	std::vector<std::string> params; // Parameters (e.g. "#cpp", "Hello!")
};

// To parse input strings into commands and parameters
class InputParser
{
	public:
		static ParsedCommand parse(const std::string& input);
};
