/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/18 11:41:39 by meharit           #+#    #+#             */
/*   Updated: 2024/05/10 11:47:39 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

int    Response::delete_file(std::string filename)
{
    if (access(filename.c_str(), F_OK | W_OK) == -1)
        return 403;
    remove(filename.c_str());
    return 204;
}

int    Response::delete_folder(std::string filename)
{
    if (filename[filename.length() - 1] != '/')
        return 409;

    if (access(filename.c_str(), R_OK) == -1)
        return 403;

    DIR* directory = opendir(filename.c_str());
    if (!directory)
        return 500;
    
    struct dirent* entry;
    while ((entry = readdir(directory)))
    {
        if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
        {
            int del_f = delete_folder(filename + std::string(entry->d_name) + "/");
            if (del_f == 403)
            {
                closedir(directory);
                return 403;
            }
            if (del_f == 500)
            {
                closedir(directory);
                return 500;
            }
        }
        if (entry->d_type == DT_REG)
        {
            if (delete_file(filename + std::string(entry->d_name)) == 403)
            {
                closedir(directory);
                return 403;
            }
        }
    }
    closedir(directory);
    
    if (access(filename.c_str(),  W_OK) == -1)
        return 403;
    
    if (rmdir(filename.c_str()) == -1)
    {
        if (access(filename.c_str(),  W_OK) == -1)
            return 403;
        return 500;        
    }
    return 204;
}

int    Response::Delete()
{
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
    
    if (type == 1) //file
    {
        status_code = delete_file(requested_resource);
        return status_code;
    }
    
    if (type == 0) //directory
    {
        if (access(requested_resource.c_str(), F_OK | W_OK) == -1)
        {
            status_code = 403;
            return 403;
        }
        
        status_code = delete_folder(requested_resource);
        return status_code;
    }
    return status_code;
}
