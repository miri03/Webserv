/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 16:22:10 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/01 16:58:34 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "configfile.hpp"
#include <map>


class location
{
private:
public:
    location();
    std::string name;
    std::string root;
    std::string index;
    bool autoindex;
    bool allow_methods;
    bool delet;
    bool get;
    bool post;
    std::string return_code;
    std::string cgi_extension;
    bool upload;
    std::string upload_path;
    std::map<std::string, std::string> cgi_conf;
    bool root_set;
    bool index_set;
    bool autoindex_set;
    bool allow_methods_set;
    bool return_code_set;
    bool upload_set;
    bool upload_path_set;
    
};

