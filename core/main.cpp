/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 17:13:48 by eahn              #+#    #+#             */
/*   Updated: 2025/04/28 18:19:50 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/Server.hpp"
#include "utils/Logger.hpp"

#include "core/Server.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    if (argc != 3) {
        Logger::error("Usage: ./ircserv <port> <password>");
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        Logger::error("Invalid port number. Must be between 1024 and 65535.");
        return 1;
    }

    std::string password = argv[2];

    try {
        Server server(port, password);
        server.run();
    } catch (const std::exception& e) {
        Logger::error(std::string("Fatal error: ") + e.what(), true);
    }

    return 0;
}
