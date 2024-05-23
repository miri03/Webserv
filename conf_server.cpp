/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 14:13:16 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/11 12:00:12 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "conf_server.hpp"

server::server()
{
    port = 0;
    host = "";
    server_host = 0;
    error_page = "";
    client_max_body_size = 0;
    client_max_body_size_set = false;
    root = "";
    index = "";
    port_set = false;
    host_set = false;
    server_name_set = false;
}

void server::pars_location(std::ifstream &file,int  &i,std::string &line,std::string &w,int &line_number)
{
    i = 0;;
    location loc;
    std::istringstream ss;
    std::string word;
    loc.name = w;
    if(loc.name[0] != '/')
        throw std::runtime_error("bad config file: NOT A PATH");
    
    std::vector<location>::iterator it;
    for(it = this->locations.begin(); it != this->locations.end(); it++)
    {
        if(it->name == loc.name)
            throw std::runtime_error("bad config file : duplicate location");
    }
  
    while (std::getline(file,line))
    {
        line_number++;
        std::istringstream ss(line);
        while(ss >> word)
        {
            if(word == "root")
            {
                if(loc.root_set == true)
                    throw std::runtime_error("bad config file : duplicate root");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing root path");
                loc.root = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                loc.root_set = true;
            }
            else if(word == "index")
            {
                if(loc.index_set == true)
                    throw std::runtime_error("bad config file : duplicate index");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing index file");
                loc.index = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                loc.index_set = true;
            }
            else if(word == "autoindex")
            {
                if(loc.autoindex_set == true)
                    throw std::runtime_error("bad config file : duplicate autoindex");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing on/off");
                if(word == "on")
                    loc.autoindex = true;
                else if (word == "off")
                    loc.autoindex = false;
                else
                    throw std::runtime_error("bad config file : invalid value");
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                loc.autoindex_set = true;
            }
            else if(word == "allow_methods")
            {
                if(loc.allow_methods_set == true)
                    throw std::runtime_error("bad config file : duplicate allow_methods");
                while(ss >> word)
                {
                    if(word == ";")
                        break;
                    if(word == "GET" &&  loc.get == false)
                        loc.get = true;
                    else if(word == "POST" && loc.post == false)
                        loc.post = true;
                    else if(word == "DELETE" && loc.delet == false)
                        loc.delet = true;
                    else
                        throw std::runtime_error("bad config file : invalid method");
                }
                if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                if(loc.get == false && loc.post == false && loc.delet == false)
                    throw std::runtime_error("bad config file :invalid method");
                loc.allow_methods = true;
                loc.allow_methods_set = true;
            }
            else if(word == "return")
            {
                if(loc.return_code_set == true)
                    throw std::runtime_error("bad config file : duplicate return");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file  : missing return code");
                loc.return_code = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file  : missing ;");
                loc.return_code_set = true;
            }
            else if(word == "cgi_conf")
            {
                if(loc.name != "/cgi")
                    throw std::runtime_error("bad config file  : you dont have permission to use cgi_conf");
                 int  j = 0;
                while(ss >> word)
                {
                    if(j == 0)
                    {
                        if(word == ";")
                            throw std::runtime_error("bad config file  : missing cgi extension");
                       loc.cgi_extension = word;
                       if(word != "py" && word != "php")
                            throw std::runtime_error("bad config file  : this server does not support this cgi extension");
                    }
                    else if(j == 1)
                    {
                        if(word == ";")
                            throw std::runtime_error("bad config file  : missing cgi path");
                        loc.cgi_conf[loc.cgi_extension] = word;
                        if(access(word.c_str(), F_OK) == -1)
                            throw std::runtime_error("bad config file  : that path is not valid");
                        if(loc.cgi_extension == "php")
                        {
                            if(word != "/usr/bin/php-cgi" && word != "/bin/php-cgi")
                                throw std::runtime_error("bad config file  : that is not a valid path for php-cgi");
                        }
                    }
                    else if (j == 2)
                    {
                        if(word == ";")
                            break;
                    }
                    else
                        throw std::runtime_error("bad config file  : invalid cgi conf");
                    j++;  
                }
                if(word != ";")
                    throw std::runtime_error("bad config file  : missing ;");
            }
            else if(word == "upload")
            {
                if(loc.upload_set == true)
                    throw std::runtime_error("bad config file : duplicate upload");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing on/off");
                if(word == "on")
                    loc.upload = true;
                else if (word == "off")
                    loc.upload = false;
                else
                    throw std::runtime_error("bad config file : invalid value");
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                loc.upload_set = true;
            }
            else if(word == "upload_path")
            {
                if(loc.upload_path_set == true)
                    throw std::runtime_error("bad config file : duplicate upload path");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing upload path");
                loc.upload_path = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                loc.upload_path_set = true;
            }
            else if(word == "}")
            {
                i = 1;
                break;
            }
            else if(word[0] == '#')
            {
                break;
            }
            else
                throw std::runtime_error("bad config file : invalid location component");
        }
            if(word == "}")
            {
                break;
            }

    }
    if(loc.name == "/cgi" && loc.cgi_conf.size() == 0)
        throw std::runtime_error("bad config file : you must have at least one cgi_conf");
    if(loc.post == true && loc.upload_path_set == false)
        throw std::runtime_error("bad config file : missing upload path");
    if(word != "}" || i != 1)
        throw std::runtime_error("bad config file : missing }");
    this->locations.push_back(loc);
}