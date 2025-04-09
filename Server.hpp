/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahn <eahn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:05:12 by eahn              #+#    #+#             */
/*   Updated: 2025/04/09 16:55:07 by eahn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server
{
    public:
        // Constructor & Destructor
        Server(int port, const std::string& password);
        ~Server();

        
    private:
        int serverSocket_;

};

#endif