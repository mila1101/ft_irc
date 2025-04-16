/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:11:02 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 00:24:12 by eahn             ###   ########.fr       */
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

	// 2. if input is empty, return empty command
	if (input.find_first_not_of(" \t") == std::string::npos)
		return result;

	std::istringstream iss(input);
	std::string word;
	
	// To DO

}
