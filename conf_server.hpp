/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 18:31:58 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/04 21:00:56 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "configfile.hpp"
#include <iostream>
#include <vector>
#include <map>

class location;
class server
{
public:
    server();
    long long port;
    std::string host;
    int server_host;
    std::vector<std::string> server_name;
    std::string error_page;
    size_t client_max_body_size;
    bool client_max_body_size_set;
    std::string root;
    std::string index;
    std::vector <location>  locations;
    std::map<int, std::string> error_pages;
    void pars_location(std::ifstream &file,int  &i,std::string &line,std::string &word,int &line_number);
    bool port_set;
    bool host_set;
    bool server_name_set;
};


