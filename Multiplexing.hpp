/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexing.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <meharit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:08:49 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/07 00:02:18 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/epoll.h>

#include <signal.h>
#include <sys/time.h>

#include "response.hpp"

class Request;
class Multiple
{
    public:
        void server(configfile &conf);
        std::map<int, Response*> resp;
        
        void    close_connection(int, int, std::map<int,Request*>&);
        void            resp_send(int fd, std::map<int,Request*> &tbd, int epollfd);

};