/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resp_location.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 09:59:28 by meharit           #+#    #+#             */
/*   Updated: 2024/05/11 12:08:16 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

void    get_matched_location_for_request_uri(Request& req, Response& resp, const configfile& conf)
{
    resp.set_content_length(req.get_Mymap("content-length"));
    resp.set_content_type(req.get_Mymap("content-type"));
    
    resp.method  = req.get_method();
    resp.version = req.get_version();
    
    resp.set_request_target(req.get_target());
    resp.multiple_slash();

    configfile& conff = const_cast<configfile&>(conf);
    int i = find_server_request(req, conff);

    std::string location;
    location = get_location(req, conf, i, resp);
    resp.requested_resource = location;
    resp.server_index = i;
    if (resp.requested_resource == "404")
    {
        resp.matched_location_error = true;   
        return;
    }
    
    /////////////////////////////CGI///////////////////////////////////////////
    
    if (resp.if_location_has_cgi(conf))
    {
        std::string tmp;
        for (int i = 0; resp.requested_resource[i] ; i++)
        {
            if (resp.requested_resource[i] == '?')
            {
                resp.Query_String = &resp.requested_resource[++i];
                break;   
            }
            tmp += resp.requested_resource[i];
        }
        if (!resp.Query_String.empty())
        {
            if (resp.requested_resource[resp.requested_resource.length() - 1] == '/')
                tmp += '/';
            resp.requested_resource = tmp;   
        }
    }

    if (!real_path(resp))
    {
        resp.requested_resource = "403";
        resp.matched_location_error = true;
        return;
    }

}

int    real_path(Response& resp)
{
    std::stringstream str;
    char Buffer[1024];
    char *temp = realpath(resp.requested_resource.c_str(), Buffer);
    if (temp)
    {
        str << temp;
        std::string tmp_resource;
        tmp_resource = str.str();
    
        if (resp.requested_resource[resp.requested_resource.length() - 1] == '/')
        {
            tmp_resource += '/';
        }
        
        if (resp.method == "DELETE")
        {
            if (resp.the_root[resp.the_root.length() - 1] != '/')
            {
                if ((resp.the_root + "/") == tmp_resource)
                return 0;
            }
            if (resp.the_root == tmp_resource)
                return 0;
        }
        for (size_t i = 0; resp.the_root[i]; i++)
        {
            if (i > tmp_resource.length())
                return 0;
        }
        resp.requested_resource = tmp_resource;
    }
    
    return 1;
}

std::string get_location_post(const Request &req, const configfile& conf, int i,int *k)
{
    int len = INT_MAX;
    int index;
    for (int tmp = 0; tmp < (int)conf.servers[i].locations.size(); tmp++)
    {
        int size = size_string(req.get_target(), conf.servers[i].locations[tmp].name);
        if (len > size && size != -1)
        {
            len = size;
            index = tmp;
        }
    }
    if (len == INT_MAX)
        return ("404");
    *k = index;
    std::string location = conf.servers[i].locations[index].name;

    if (conf.servers[i].locations[index].root.empty() && conf.servers[i].root.empty())
        return ("404");

    int root_size;
    std::string new_root;

    if (!conf.servers[i].locations[index].root.empty())
    {
        root_size = conf.servers[i].locations[index].root.length();
        new_root = conf.servers[i].locations[index].root;
        if (conf.servers[i].locations[index].root[root_size - 1] == '/')
            new_root = conf.servers[i].locations[index].root.substr(0, root_size - 1);
    }
    else if (conf.servers[i].locations[index].root.empty())
    {
        root_size = conf.servers[i].root.length();
        new_root = conf.servers[i].root;
        if (conf.servers[i].root[root_size - 1] == '/')
            new_root = conf.servers[i].root.substr(0, root_size - 1);
    }
    
    if (location == "/")
    {
        if (conf.servers[i].locations[index].root.empty())
            return (new_root + req.get_target());
        return (new_root + req.get_target());
    }

    int size = conf.servers[i].locations[index].name.length();
    if (conf.servers[i].locations[index].root.empty())
        return (new_root + &req.get_target()[size]);
    return (new_root + &req.get_target()[size]);
}

std::string get_location(const Request &req, const configfile& conf, int i, Response& resp)
{
    int len = INT_MAX;
    int index;
    for (int tmp = 0; tmp < (int)conf.servers[i].locations.size(); tmp++)
    {
        int size = size_string(req.get_target(), conf.servers[i].locations[tmp].name);
        if (len > size && size != -1)
        {
            len = size;
            index = tmp;
        }
    }
    if (len == INT_MAX)
    {
        resp.set_status_code(404);
        resp.matched_location_error = true;
        return ("404");
    }

    resp.index_location = index;
    std::string location = conf.servers[i].locations[index].name;

    if (conf.servers[i].locations[index].root.empty() && conf.servers[i].root.empty())
    {
        resp.set_status_code(404);
        resp.matched_location_error = true;
        return ("404");
    }

    int root_size;
    std::string new_root;

    if (!conf.servers[i].locations[index].root.empty())
    {
        root_size = conf.servers[i].locations[index].root.length();
        new_root = conf.servers[i].locations[index].root;
        if (conf.servers[i].locations[index].root[root_size - 1] == '/')
            new_root = conf.servers[i].locations[index].root.substr(0, root_size - 1);
    }
    else if (conf.servers[i].locations[index].root.empty())
    {
        root_size = conf.servers[i].root.length();
        new_root = conf.servers[i].root;
        if (conf.servers[i].root[root_size - 1] == '/')
            new_root = conf.servers[i].root.substr(0, root_size - 1);
    }
    
    resp.the_root = new_root;
    
    if (location == "/")
    {
        if (conf.servers[i].locations[index].root.empty())
            return (new_root + req.get_target());
        return (new_root + req.get_target());
    }

    int size = conf.servers[i].locations[index].name.length();

    if (conf.servers[i].locations[index].root.empty())
        return (new_root + &req.get_target()[size]);
    return (new_root + &req.get_target()[size]);
}

int find_server_request(const Request &req,  configfile& conf)
{
    int i = 0;
    int pos = req.get_Mymap("host").find(":", 0);
    std::string server_request = req.get_Mymap("host").substr(0, pos);

    std::string port_s = req.get_Mymap("host").substr(pos + 1, req.get_Mymap("host").length());
    long port = atoll(port_s.c_str());

    if (server_request == "localhost")
        server_request = "127.0.0.1";
    bool is_ip = false;
    try
    {
        conf.ft_inet_add(server_request);
        is_ip = true;
    }
    catch(const std::exception& e)
    {
        is_ip = false;
    }
    if(is_ip == false)
    {
        for(; i < (int)conf.servers.size(); i++)
        {
           for(int j = 0;j < (int)conf.servers[i].server_name.size(); j++)
           {
               if (conf.servers[i].server_name[j] == server_request)
               {
                        is_ip = true;
                       break;
               }
           }
              if (is_ip)
                break;
        }
    }
    else
    {
        for (; i < (int)conf.servers.size(); i++)   //find index of the server
        {
            if (conf.servers[i].host == server_request)
            {
                if (port == conf.servers[i].port)
                    break;
            }
                
        }
    }
    if (i == (int)conf.servers.size()) 
        i = 0;
    return (i);
}
