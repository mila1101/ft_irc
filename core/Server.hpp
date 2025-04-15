/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/15 16:01:31 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <netinet/in.h> // sockaddr_in
#include <poll.h> // poll
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h> // inet_ntoa
#include <fcntl.h> // fcntl
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
        void logMessage(LogLevel level, const std::string& message, bool exitAfter = false);

};
