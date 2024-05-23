/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 18:58:02 by meharit           #+#    #+#             */
/*   Updated: 2024/05/12 11:50:51 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include <fcntl.h>


std::string Response::mime(std::string file)
{
    for (int i = file.length(); i > 0; i--)
    {
        if (file[i] == '.')
        {
            if (mimes[&file[i]].empty())
            {
                not_supported = true;
                return ("text/plain");
            }
            return (mimes[&file[i]]);
        }
    }
    not_supported = true;
    return ("text/plain");
}

std::string Response::extension_cgi(std::string file)
{
    for (int i = file.length(); i > 0; i--)
    {
        if (file[i] == '.')
        {
            
            if (strcmp(&file[i], ".py") == 0)
                return (".py");
            if (strcmp(&file[i], ".php") == 0)
                return (".php");
        }
    }
    return ("");
}

int Response::if_location_has_cgi(const configfile& conf)
{
    if (conf.servers[server_index].locations[index_location].name == "/cgi" || requested_resource.find("/cgi/") != std::string::npos)
        return 1;
    return 0;
}

void    Response::cgi_get_file(const configfile& conf)
{
    is_cgi = true;
    if(!cgi1->is_done)
        cgi1->exec_cgi(conf, *this);
    if (cgi1->is_done)
    {
        if (header_flag == false)
        {
            requested_resource = cgi1->filename;
            status_code = cgi1->cgi_status;
            if(!server_error(status_code))
            {
                if (access(requested_resource.c_str(), R_OK) == -1)
                {
                    status_code = 403;
                    return ;
                }
                _file.open(requested_resource.c_str(), std::ios::in);
                if (_file.is_open() == false)
                {
                    status_code = 500;
                    return;
                }
                set_status_line();
                header += status_line;
                my_response = (char*)header.c_str();
                send_size = header.size();
                header_flag = true;
            }
            //write the header
        }
        else if (!server_error(status_code))
        {
            
            bzero(bod, 1024);
            _file.read(bod, 1024);
            send_size = _file.gcount();
            my_response = bod;
            
            read_flag = true;
            send_flag = false;
        }
        
        if (_file.eof())
        {
            done = true;
            _file.close();
        }
        if (done || server_error(status_code))
        {
            remove(cgi1->filename.c_str());
            remove(cgi1->cgi_file_posted.c_str());
        }
    }
}

int Response::get_file(const configfile& conf)
{
    if (access(requested_resource.c_str(), R_OK) == -1)
    {
        status_code = 403;
        return 403;
    }
    std::string extension = extension_cgi(requested_resource);
    if (error_flag == false  && if_location_has_cgi(conf) && (extension == ".php" || extension == ".py" ))
    {
        cgi_get_file(conf);
        return status_code;
    }
    else
    {
        if (header_flag == false)
        {
            _file.open(requested_resource.c_str(), std::ios::in);
            if (_file.is_open() == false)
                return 500;
        }
        if (!not_supported && send_flag == true && read_flag == false)
        {
            std::string bdy;

            bzero(bod, 1024);
            bzero(my_response, send_size);
            resp_string.clear();
            
            _file.read(bod, 1024);
            send_size = _file.gcount();

            bdy.append(bod, send_size);
            
            std::stringstream size;
            size << std::hex << bdy.length();
            
            resp_string += size.str();
            resp_string += "\r\n";
            resp_string += bdy;
            resp_string += "\r\n";

            read_flag = true;
            send_flag = false;
        }
        set_content_type(mime(requested_resource));
        if (_file.eof())
        {
            resp_string += "0\r\n\r\n";
            done = true;
        }
        if (header_flag == true)
        {
            my_response = const_cast<char*>(resp_string.c_str());
            send_size = resp_string.length();
        }
        if (not_supported)
            return 415;
        return 200;
    }
    
}

void    Response::auto_index(DIR* directory)
{
    if (send_flag == true && read_flag == false)
    {
        body = "<html>\n<head><meta charset=\"UTF-8\"><title>Index of ";
        body += requested_resource;
        body += "</title></head>\n<body>\n<h1>Index of ";
        body += requested_resource;
        body += "</h1><hr><pre>\n";

        struct dirent* entry;
        while ((entry = readdir(directory)))
        {
            if (strcmp(entry->d_name, "."))
            {
                body += "<a href=\"";
                body += entry->d_name;
                body += "\">";
                body += entry->d_name;
                body += "</a>\n";
            }
        }
        body += "</pre><hr></body>\n</html>";
    
        std::stringstream size;
        
        size << std::hex << body.length();
        
        resp_string += size.str();
        resp_string += "\r\n";
        resp_string += body;
        resp_string += "\r\n";
        
        resp_string += "0\r\n\r\n";

        my_response = const_cast<char*>(resp_string.c_str());
        send_flag = false;
        read_flag = true;
        done = true;
        send_size = resp_string.length(); 
    }
    set_content_type("text/html");
    closedir(directory);
}

int Response::get_folder(const configfile& conf)
{
    if (requested_resource[requested_resource.length() - 1] != '/')
    {
        requested_resource = request_target + "/";
        read_flag = true;
        return 301;
    }
    index_file = conf.servers[server_index].locations[index_location].index;
    if (conf.servers[server_index].locations[index_location].index.empty()) //if no index in the location
    {
        if (conf.servers[server_index].index.empty()) //if no index in the server
        {
            if (!conf.servers[server_index].locations[index_location].autoindex) //if no auto_index in the location
                return 403;
            //if there is auto_index in the location

            DIR* directory = opendir(requested_resource.c_str());
            if (!directory)
                return 403;

            auto_index(directory);
            return 200;
        }
        index_file = conf.servers[server_index].index; //if there is index in the server
    }
    //if there is index in the location

    requested_resource += index_file;
    int type = resource_type();
    
    if (type == -2)
    {
        status_code = 500;
        return 500;
    }
    if (type == -1)
    {
        status_code = 404;
        return 404;
    }
    if (access(requested_resource.c_str(), R_OK) == -1)
    {
        status_code = 403;
        return 403;
    }
    if (type == 0) //if index_file is a folder
    {
        status_code = 403;
        return 403;
    }
    get_file(conf);
    type_resource = is_file;
    return 200;
}

int Response::Get(const configfile& conf)
{
    int type = INT_MAX;
    if (header_flag == false)
        type = resource_type();
    
    if (type == -2)
    {
        status_code = 500;
        return status_code; 
    }

    if (type == -1)
    {
        status_code = 404;
        return 404; 
    }
    
    if (type == 1 || type_resource == is_file) //file
    {
        type_resource = is_file;
        status_code = get_file(conf);
        return status_code;
    }
    
    if (type == 0 || type_resource == is_folder) //directory
    {
        type_resource = is_folder;
        status_code = get_folder(conf);
        return status_code; 
    }
    return status_code;
}
