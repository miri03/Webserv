/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configfile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 15:56:49 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/11 11:42:55 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "configfile.hpp"

void configfile::pars_configfile(char *filename,std::ifstream &a)
{
    int i  = 0;
    line_number = 0;
    
    a.open(filename);
    if(!a)
        throw std::runtime_error("file not found");
    else
    {
        std::string line;
        std::string word;
        while(std::getline(a, line))
        {
            line_number++;
            std::istringstream ss(line);
            if(line == "server {")
            {
                i++;
                pars_server(a,line);
            }
            else if(line == "\0")
                continue;
            else if(ss >> word && word[0] == '#')
                continue;
            else if(checkline(line))
                continue;
            else
                throw std::runtime_error("bad config file: missing server {");
            
        }
        a.close();
        if(i == 0)
            throw std::runtime_error("bad config file : missing server {");
        check_the_duplicat();
    }
}

bool configfile::checkline(std::string &line)
{
    for(size_t i = 0; i < line.size(); i++)
    {
        
        if(line[i] == ' ' || line[i] == '\n')
            continue;
        else
            return false;
    }
    return true;
}
void configfile::pars_server(std::ifstream &file, std::string &line)
{
    int  i = 0 , j = 0;
    server serv;
    std::istringstream ss;
    std::string word;
    while (std::getline(file,line))
    {
        
        line_number++;
        std::istringstream ss(line);
        while(ss >> word)
        {   
            if(word == "listen")
            {
                if(serv.port_set == true)
                    throw std::runtime_error("bad config file : duplicate port");
                ss >> word;
                check_port(word);
                serv.port = std::atoll(word.c_str());
                check_port(serv.port);
                ss >> word;
                if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                i++;
                serv.port_set = true;
            }
            else if(word == "host")
            {
                if(serv.host_set == true)
                    throw std::runtime_error("bad config file : duplicate host");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing host name");
                serv.host = word;
                if(serv.host == "localhost")
                    serv.host = "127.0.0.1";
                check_host(serv.host);
                serv.server_host = ft_inet_add(serv.host);
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                i++;
                serv.host_set = true;
            }
            else if(word == "server_name")
            {
                if(serv.server_name_set == true)
                    throw std::runtime_error("bad config file : duplicate server name");
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing server name");
                do
                {
                    if(word == ";")
                        break;
                    serv.server_name.push_back(word);
                } while (ss >> word);
                
                if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                i++;
                serv.server_name_set = true;
                
            }
            else if(word == "error_page")
            {
                int  j = 0;
                while(ss >> word)
                {
                    if(j == 0)
                    {
                        
                        if(word == ";")
                            throw std::runtime_error("bad config file : missing error code");
                        check_error_page(word);
                        serv.error_page = word;
                    }
                    else if(j == 1)
                    {
                        if(word == ";")
                            throw std::runtime_error("bad config file : missing error page path");
                        serv.error_pages[std::atoi(serv.error_page.c_str())] = word;
                        size_t i = word.rfind(".html");
                        if(i == std::string::npos || word.size() - i != 5)
                            throw std::runtime_error("bad config file : invalid error page");
                    }
                    else if (j == 2)
                    {
                        if(word == ";")
                            break;
                    }
                    else
                        throw std::runtime_error("bad config file : invalid error page conf");
                    j++;  
                }
                if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
            }
            else if(word == "client_max_body_size")
            {
                ss >> word;
                check_port(word);
                serv.client_max_body_size = std::atoi(word.c_str());
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
                serv.client_max_body_size_set = true;
            }
            else if(word == "root")
            {
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing root path");
                serv.root = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
            }
            else if(word == "index")
            {
                ss >> word;
                if(word == ";")
                    throw std::runtime_error("bad config file : missing index file");
                serv.index = word;
                ss >> word;
                 if(word != ";")
                    throw std::runtime_error("bad config file : missing ;");
            }
            else if(word == "location")
            {
                ss >> word;
                int j = 0;
                if(word == "{")
                    throw std::runtime_error("bad config file : missing location name");
                serv.pars_location(file,j,line,word,line_number);
                ss >> word;
                    if(word != "{")
                        throw std::runtime_error("bad config file : missing {");
            }
            else if(word == "}")
            {
                j = 1;
                break;
            }
            else if(word[0] == '#')
            {
                break;
            }
            else
                throw std::runtime_error("bad config file : invalid server conf");
        }
            if(word == "}")
            {
                break;
            }
    }
    if(word != "}" || j != 1)
        throw std::runtime_error("bad config file : missing }");
    if(i != 3)
        throw std::runtime_error("bad config file : missing server conf");
    this->servers.push_back(serv);
    
}

void configfile::check_port(std::string &port)
{
    
    for (size_t i = 0; i < port.size(); i++)
    {
        if(!isdigit(port[i]))
            throw std::runtime_error("bad config file :not a number");
    }
}

void configfile::check_port(long long &port)
{
    if(port <= 1023 || port > 65535)
        throw std::runtime_error("bad config file : invalid port");
}

void configfile::check_error_page(std::string &error_page)
{
    int num = 0;
    check_port(error_page);
    num = std::atoi(error_page.c_str());
    if((num >= 400 && num <= 417) || (num >= 500 && num <= 505))
        return ;
    else
        throw std::runtime_error("bad error page : invalid error code");
  
}

void configfile::check_host(std::string &host)
{
    int num = 0;
    for (size_t i = 0; i < host.size(); i++)
    {
       if(host[i] == '.')
            num++;
    }
    if(num != 3)
        throw std::runtime_error("bad config file : invalid host");
}

int configfile::ft_inet_add(std::string &ip)
{
    size_t i = 0;
    int k = 0;
    int res = 0;
    int tmp = 0;
    int count = 0;
    bool flag = false;
    while(i <= ip.size())
    {
        if(ip[i] == '.' || i == ip.size())
        {
            if(tmp > 255)
                throw std::runtime_error("Invalid ip address");
            count++;
            res =  res | (tmp << (8 * k));
            k++;
            tmp = 0;
            if(flag == true)
                throw std::runtime_error("Invalid ip address");
            flag = true;
        }
        else if(isdigit(ip[i]))
        {
            tmp = tmp * 10 + (ip[i] - '0');
            flag = false;
        }
        else
            throw std::runtime_error("Invalid ip address");
        i++;
    }
    if(count != 4 || i > 16 || res == 0)
        throw std::runtime_error("Invalid ip address");
    return (res);
}

void configfile::check_the_duplicat()
{
    int checker = 0;
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        for (size_t j = 0; j < this->servers.size(); j++)
        {
            if(i != j)
            {
                if(this->servers[i].port == this->servers[j].port)
                    checker++;
                if(this->servers[i].server_host == this->servers[j].server_host)
                    checker++;
                for(size_t k = 0; k < this->servers[i].server_name.size(); k++)
                {
                    for(size_t l = 0; l < this->servers[j].server_name.size(); l++)
                    {
                        if(this->servers[i].server_name[k] == this->servers[j].server_name[l])
                            checker++;
                    }
                }
                if(checker >= 3)
                    throw std::runtime_error("duplicat server conf");
            }
            checker = 0;
        }
    }
}
