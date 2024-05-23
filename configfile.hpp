/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configfile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 15:56:54 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/10 11:14:22 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include "conf_server.hpp"
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include "location.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class server;
class configfile
{
private:
    public:
    std::vector<server>  servers;
    void pars_configfile(char *filename,std::ifstream &a);
    void   pars_server(std::ifstream &file,std::string &line);
    void check_port(std::string&);
    void check_port(long long &);
    void check_error_page(std::string&);
    int line_number;
    void check_host(std::string&);
    int ft_inet_add(std::string &ip);
    void check_the_duplicat();
    bool checkline(std::string &line);
};



