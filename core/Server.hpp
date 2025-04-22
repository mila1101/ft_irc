/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/22 15:02:24 by eahn             ###   ########.fr       */
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
#include <sys/socket.h> // for send
#include <map>

#include "../utils/Logger.hpp"
#include "../client/Client.hpp"
#include "../client/Channel.hpp"

class SocketHandler;
class CommandHandler;

class Server
{
    public:
        // Constructor & Destructor
        Server(int port, const std::string& password);
        ~Server();

        // Server lifecycle
        void run();
        void stop();

        // Send to client
        void Server::msgClient(int clientSocket, const std::string & msg);

        // Getters
        std::string getIP() const;
        std::string getServerName() const;

        Client& getClient(int fd);
        std::map<int, Client>& getClients();

        int getClientFdByNickName(const std::string& nick) const;

        Channel& getChannel(const std::string& name);
        std::map<std::string, Channel>& getChannels();
        Channel& getOrCreateChannel(const std::string& name);

        // Client / Channel management
        void removeClient(int fd);
        void removeClientFromChannel(int fd, const std::string& channelName);
        
        // Welcome message
        void sendWelcome(int fd, const Client& client);

    private:
        // Network state
        int port_;
        std::string password_;
        int listenFd_;
        bool running_;
        sockaddr_in serverAddr_;
        std::vector<struct pollfd> pollFds_;

        // Server logic
        std::unique_ptr<CommandHandler> commandHandler_;
        std::unique_ptr<SocketHandler> socketHandler_;

        // Core data
        std::string serverIp_;
        std::string serverName_;
        std::map<int, Client> clients_;
        std::map<std::string, Channel> channels_;
        
        // Internal helpers
        bool initSocket(); // Init server socket
        void setupPoll(); // Init pollFds
        void pollLoop();
        void handleIncomingConnection(); // When event on server socket
        void handleClientMessage(int fd); // When event on client socket

        // Signal handling  
        void setupSignalHandler(); // Setup signal handler
        static void handleSignal(int signal); // static handler
        static Server* instance_; // Singleton pointer so that signal handler can access instance
};
