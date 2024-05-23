/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resp_uti.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 17:44:23 by meharit           #+#    #+#             */
/*   Updated: 2024/05/11 20:45:46 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

int  Response::check_methods(const configfile& conf)
{
    if (conf.servers[server_index].locations[index_location].get == 0 && method == "GET" )
        return 0;
    if (conf.servers[server_index].locations[index_location].delet == 0 && method == "DELETE" )
        return 0;
    if (conf.servers[server_index].locations[index_location].post == 0 && method == "POST" )
        return 0;
    return 1;
}

int size_string(std::string request, std::string location)
{
    if (request.length() < location.length())
        return (-1);
    int i, j = 0; 
    for (i = 0; location[i]; i++)
    {
        if (location[i] != request[i])
            return (-1);
    }
    if (request[i] == 0 || location == "/" || request[i] == '/' || request[i] == '?')
        for (j = 0; request[i++]; j++){}
    else
        return -1;
    return (j);
}

int success(int status_code)
{
    if (status_code == 200 || status_code == 201 || status_code == 202 || status_code == 203 || status_code == 204 || status_code == 205 || status_code == 206)
        return 1;
    return 0;
}

int server_error(int status_code)
{
    if (status_code == 500 || status_code == 501 || status_code == 502 || status_code == 503 || status_code == 504 || status_code == 505)
        return 1;
    return 0;
}

int redirection(int status_code)
{
    if (status_code == 300 || status_code == 301 || status_code == 302 || status_code == 303 || status_code == 304 || status_code == 305 || status_code == 306 || status_code == 307)
        return 1;
    return 0;
}
