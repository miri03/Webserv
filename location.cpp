/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 16:22:47 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/02 14:51:13 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location()
{
    name = "";
    root = "";
    index = "";
    autoindex = false;
    allow_methods = false;
    delet = false;
    get = false;
    post = false;
    return_code = "";
    cgi_extension = "";
    upload = false;
    upload_path = "";
    root_set = false;
    index_set = false;
    autoindex_set = false;
    allow_methods_set = false;
    return_code_set = false;
    upload_set = false;
    upload_path_set = false;
    
}
