/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputParser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 00:11:05 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 22:55:23 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This one is to be used for Command Dispatcher (Siria?)

#pragma once

#include <string>
#include <vector>
#include <iostream>

// To store parsed command and its parameters
struct ParsedCommand
{
	std::string command; // Commands (e.g. "JOIN", "PRIVMSG")
	std::vector<std::string> params; // Parameters (e.g. "#cpp", "Hello!")


    // ✅ 디버깅 출력 함수
    void debugPrint() const {
        std::cout << "[ParsedCommand Debug]\n";
        std::cout << "  Command: " << command << "\n";
        std::cout << "  Params (" << params.size() << "):\n";
        for (size_t i = 0; i < params.size(); ++i) {
            std::cout << "    [" << i << "]: " << params[i] << "\n";
        }
        std::cout << std::endl;
    }
};

// To parse input strings into commands and parameters
class InputParser
{
	public:
		static ParsedCommand parse(const std::string& input);
};
