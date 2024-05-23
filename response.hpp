/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 18:39:46 by meharit           #+#    #+#             */
/*   Updated: 2024/05/11 20:41:13 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/types.h>
#include <dirent.h> //opendir
#include <limits.h>
#include <cstdio> // remove
#include <cstring> //strlen
#include <unistd.h> //rmdir
#include <errno.h>

#include "request.hpp"
#include "configfile.hpp"
#include "cgi.hpp"

#define is_file 1
#define is_folder 2

class cgi;
class Response
{
    private:
        
        std::string     index_file;
        
        std::string     status_line;
        std::string     content_type;
        std::string     content_length;
        
        std::string     body;
        char            bod[1024];
        char*           my_response;
        std::string     header;
        
        std::map<int, std::string> code_response;

        bool             not_supported;
        int              type_resource;

        
    public :

        std::string     resp_string;
        std::fstream _file;
        cgi* cgi1;
        struct timeval  start;
        struct timeval  end;
        bool            first_time;
        int             fd;
        bool            close_conx;        
        bool            is_cgi;
        bool            matched_location_error;
        bool            time_out;
        bool            error_flag;
        
        std::string      the_root;
        std::string     request_target;
        int             status_code;
        bool             header_flag;
        bool             send_flag;
        bool             read_flag;
        bool             done;
        
        int             send_size;
    
        int             index_location;
        std::string     method;
        std::string     version;
        std::string     requested_resource;
        std::string     Query_String;
        int             cgi_index;
        std::map<std::string, std::string> mimes;
        int             server_index;
        Response();
        ~Response();
    ///////////////////MEMBER FUNCTIONS/////////////////////////////////
        int             check_methods(const configfile& conf);
        int             allowed_moved(const configfile&);
        void            code_reason();
        void            mime_map();
        std::string     mime(std::string);
        std::string     extension_cgi(std::string);
        
        void            set_error_response(const configfile&);
        void            set_error_response( configfile&);

        
        int             delete_file(std::string);
        int             delete_folder(std::string);
        int             resource_type();

        int             get_file(const configfile& conf);
        void            cgi_get_file(const configfile& conf);
        
        int             get_folder(const configfile&);

        int             if_location_has_cgi(const configfile&);
    

        int             Delete();

        int             Get(const configfile&); 
        void            auto_index(DIR*);      
        void             multiple_slash();
        void            methods(Request&, const configfile&);

    ///////////////////SETTERS AND GETTERS/////////////////////////////////
        void            set_status_line();
        void            set_content_type(std::string);
        void            set_content_length(std::string);
        void            set_my_response(const configfile&);
        void            set_body();
        void            set_status_code(int);
        void            time_out_response(const configfile&);
        
        std::string     get_status_line();
        std::string     get_content_type();
        std::string     get_content_length();
        std::string     get_method();
        std::string     get_body();
        const char*     get_my_response();
        std::string     get_code_response(int);
        void            set_request_target(std::string);
};

void        decrypt_uri(std::string& target);

int         success(int);
int            server_error(int);
int         redirection(int);
int         size_string(std::string request, std::string location);

int         real_path(Response&);
void        get_matched_location_for_request_uri(Request&, Response& , const configfile& );
std::string get_location(const Request&, const configfile&, int, Response&);
int         find_server_request(const Request &req, const configfile& conf);
std::string get_location_post(const Request &req, const configfile& conf, int i,int *k);
int         find_server_request(const Request &req,configfile& conf);
