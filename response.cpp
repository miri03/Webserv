/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 15:58:47 by meharit           #+#    #+#             */
/*   Updated: 2024/05/12 11:52:16 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

void    Response::methods(Request& req, const configfile& conf)
{
    if (!header_flag || matched_location_error || is_cgi)
        get_matched_location_for_request_uri(req, *this, conf);
    
    if (requested_resource == "404" || requested_resource == "500" || requested_resource == "403")
    {
        status_code = atoi(requested_resource.c_str());
        set_my_response(conf);
        return ;
    }

    decrypt_uri(requested_resource);

    if (!allowed_moved(conf))
        return ;

    if (req.get_method() == "GET")
    {
        if(!cgi1->is_in && req.get_Mymap("cookie") != "\0")
            cgi1->cookie = req.get_Mymap("cookie");
        Get(conf);
        if (!is_cgi || (cgi1->is_done && server_error(status_code)))
            set_my_response(conf);
        return ; 
    }
    else if (req.get_method() == "POST")
    {
        std::string extension = extension_cgi(requested_resource);
        
        if(if_location_has_cgi(conf) && (extension == ".php" || extension == ".py" ))
        {
            cgi1->cgi_file_posted = req.file_posted;
            if(!cgi1->is_in && req.get_Mymap("cookie") != "\0")
                cgi1->cookie = req.get_Mymap("cookie");
            get_file(conf);
            if (cgi1->is_done && server_error(status_code))
                set_my_response(conf);
            return ;
        }
        set_status_code(201);
        set_my_response(conf);
        
        return ;
    }

    else if (req.get_method() == "DELETE")
    {
        Delete();
        set_my_response(conf);
        return ;
    }
}

void    Response::set_error_response(const configfile& conf)
{
    if (server_index != 400 && server_index > -1)
    {
        std::map<int, std::string>::const_iterator err_page = conf.servers[server_index].error_pages.find(status_code);
        if (err_page != conf.servers[server_index].error_pages.end() && !access(err_page->second.c_str(), F_OK | R_OK))
        {
            mime(err_page->second);
            if (!not_supported)
            {
                std::string save = requested_resource;
                int         save_status_code = status_code;
                
                requested_resource = err_page->second;
                if (!header_flag)
                {
                    _file.open(requested_resource.c_str(), std::ios::in);
                    if (_file.is_open())
                    {
                        set_content_type(mime(requested_resource));
                        header += "Transfer-Encoding: chunked\r\n";
                        content_length = "0\r\n";
                    }
                }

                if (header_flag)
                {
                    if (_file.is_open())
                    {
                        error_flag = true;
                        get_file(conf);
                        done = true;
                    }
                }
                
                status_code = save_status_code;
                requested_resource = save;
            }
        }
    }
    if (header_flag == true && !error_flag)
    {
        set_body();
        done = true;   
    }
}

int    Response::allowed_moved(const configfile& conf)
{
    //Moved Permanently 
    if (!conf.servers[server_index].locations[index_location].return_code.empty())
    {
        requested_resource = conf.servers[server_index].locations[index_location].return_code;
        status_code = 301;
        set_my_response(conf);
        return 0;
    }
    //method not allowed
    if (!check_methods(conf))
    {
        status_code = 405;
        set_my_response(conf);
        return 0;
    }
    return 1;
}

void    Response::multiple_slash()
{
    int j = 0;

    for (; (size_t)j < request_target.size(); j++)
    {
        int i = 0;
        for (; request_target[j+i] == '/'; i++);
        if (i > 1)
            request_target = request_target.erase(j+1, i-1);
    }
}

int    Response::resource_type()
{
    if (access(requested_resource.c_str(), F_OK) == -1)
        return -1;

    DIR* directory = opendir(requested_resource.c_str());
    if (!directory)
    {
        if (errno == ENOTDIR)
            return 1;
            
        if (errno == EACCES)
            return 0;//is directory no permission
        return -2;
    }

    closedir(directory);
    return 0;//is directory
}
