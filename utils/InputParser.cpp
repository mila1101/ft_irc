/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:11:02 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 22:52:05 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InputParser.hpp"
#include <sstream> // for std::istringstream

ParsedCommand InputParser::parse(const std::string& rawInput)
{
	ParsedCommand result;

	// 1. Remove \r\n
	std::string input = rawInput;
	if (input.size() >= 2 && input.substr(input.size() - 2) == "\r\n")
		input = input.substr(0, input.size() - 2);

	// 2. Skip empty/whitespace-only lines
	if (input.find_first_not_of(" \t") == std::string::npos)
		return result;

	std::istringstream iss(input);
	std::string word;
	
	while (iss >> word)
	{
		// 3. Handle trailing param (starts with ":")
		if (word[0] == ':') 
		{
			std::string trailing;
			std::getline(iss, trailing);
			word.erase(0, 1);
			result.params.push_back(word + trailing);
			break;
		}

		if (result.command.empty())
		{
			result.command = word;

			// Convert command to uppercase
			std::transform(result.command.begin(), result.command.end(), 
				result.command.begin(), ::toupper);
		}
		else
		{
			result.params.push_back(word);
		}
	}
	return result;
}
