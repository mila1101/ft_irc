/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/17 00:06:32 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <csignal> // sigaction, sigemptyset
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h> // inet_ntoa
#include <fcntl.h> // fcntl
#include <unordered_map> //for commands
#include "../utils/Logger.hpp"

class SocketHandler;

class Server
{
    public:
        // Constructor & Destructor
        Server(int port, const std::string& password);
        ~Server();

        // Server lifecycle
        void run();
        void stop();

    private:
        int port_;
        std::string password_;
        int listenFd_;
        bool running_;

        sockaddr_in serverAddr_;
        std::vector<struct pollfd> pollFds_;

        std::unique_ptr<SocketHandler> socketHandler_;

        bool initSocket(); // Init server socket
        void setupPoll(); // Init pollFds
        void pollLoop();

        void handleIncomingConnection(); // When event on server socket
        void handleClientMessage(int fd); // When event on client socket

        // Signal handling  
        void setupSignalHandler(); // Setup signal handler
        static void handleSignal(int signal); // static handler
        static Server* instance_; // Singleton pointer so that signal handler can access instance

        // Commad table variable
        using cmdFunction = void (Server::*)(int, const std::string&);
        std::unordered_map<std::string, std::function<void(int, std::string const&)>> commandTable;

        // Commands
        void addCommand(const std::string& name, cmdFunction handler);
        void commandInit();
};
