/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:42:00 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/09 21:57:01 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <map>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>
#include "response.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <sys/time.h>
class Response;
class cgi
{
public:

    ~cgi();
    std::map<std::string, std::string> cgi_env;
    char **env;
    std::string path;
    FILE *file;
    struct timeval start;
    struct timeval end;
    std::string filename;
    std::string cgi_content_type;
    int cgi_status;
    int pid;
    bool is_in;
    bool is_done;
    int status;
    std::string extension;
    std::string   cgi_file_posted;
    std::string cookie;
    

    
    void set_env(const configfile& conf, Response& res);
    void exec_cgi(const configfile& conf, Response& res);
    int check_if_i_have_it(const configfile& conf, Response& res,std::string mine);
    std::string generateRandomFileName();
    int parsing_file_cgi();
};
