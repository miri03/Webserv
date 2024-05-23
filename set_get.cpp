/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_get.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 19:10:02 by meharit           #+#    #+#             */
/*   Updated: 2024/05/12 11:58:52 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"


Response::~Response()
{
    delete cgi1;
}

Response::Response()
{
    code_reason();
    mime_map();
    status_code = 0;
    version = "HTTP/1.1 ";
    content_length = "0\r\n";
    content_type = "text/html\r\n";
    body = "\r\n";
    header_flag = false;
    send_flag = false;
    read_flag = false;
    done = false;
    server_index = -1;
    is_cgi = false;
    not_supported = false;
    close_conx = false;
    matched_location_error = false;
    time_out = false;
    error_flag = false;
    type_resource = 0;

    cgi1 = new cgi();
    cgi1->is_in = false;
    cgi1->is_done = false;
    cgi1->status = 0;
    cgi1->cgi_status = 200;
    cgi1->pid = 1;
    cgi1->filename = "";
    cgi1->path = "";
    cgi1->file = NULL;
    cgi1->env = NULL;
    cgi1->cookie = "";
    cgi1->start.tv_sec = 0;
    cgi1->end.tv_sec = 0;

    first_time = false;
    start.tv_sec = 0;
    end.tv_sec = 0;
    

}

void    Response::set_status_line()
{   
    std::stringstream str;
    str << version << " " << status_code << " " << code_response[status_code] << "\r\n";
    status_line = str.str();
}

void    Response::set_content_length(std::string length)
{
    if (length != "\0")
    {
        content_length = length;
        content_length += "\r\n";
    }
}

void    Response::set_content_type(std::string type)
{
    if (type != "\0")
    {
        content_type = type;
        content_type += "\r\n";    
    }
}

void    Response::set_my_response(const configfile& conf)
{
    
    if (header_flag == false)
    {
        set_status_line();
        header += status_line;

        if (status_code == 204 || status_code == 201)
        {
            header_flag = true;
            done = true;
            header += "\r\n";   
            send_size = header.size();
            my_response = (char*)header.c_str();
            return ;
        }
        
        if (!success(status_code))
        {
            if (status_code == 501 && method == "HEAD")
            {
                header_flag = true;
                done = true;
                header += "\r\n";   
                send_size = header.size();
                my_response = (char*)header.c_str();
                return;
            }
            set_content_type("text/html");
            
            // 480 + reason
            int i = 480 + code_response[status_code].size();
            std::stringstream tmp ;
            tmp << i;
            set_content_length(tmp.str());
            
            set_error_response(conf);
        }
        
        if (!content_type.empty()) //not empty
        {
            header += "Content-Type: ";
            header += content_type;
        }
        
        if (success(status_code))
        {
            // if (!is_cgi)
            
                header += "Transfer-Encoding: chunked\r\n";
        }

        if (redirection(status_code))
        {
            header_flag = true;
            header += "Location: ";
            requested_resource += "\r\n";
            header += requested_resource;
            done = true;
        }
        if (content_length != "0\r\n" )
        {
            header += "Content-Length: ";
            header += content_length;   
        }
        
   

            header += "\r\n";
        
        
        my_response = (char*)header.c_str();
        send_size = header.size();
        header_flag = true;
    }
    
    else if (header_flag == true)
    {
        if (!success(status_code))
            set_error_response(conf);
    }
}

void    Response::set_body()  //error_page
{
    body = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>Error</title>\n<style>\nbody {\
            \nfont-family: \"Times New Roman\", Times, serif;\nbackground-color: #FFFFFF;\nmargin: 0;\npadding: 0;\
            \ndisplay: flex;\njustify-content: center;\nalign-items: center;\nheight: 100vh;\n}\
            \ndiv {\ntext-align: center;\ncolor: #607d3b;\n}\nh1 {\nfont-size: 3em;\n}\
            \np {\nfont-size: 1.2em;\n}\n</style>\n</head>\n<body>\n<div>\
            \n<h1>error_code</h1>\n<p>reason</p>\n</div>\n</body>\n</html>";
    
    int find = body.find("error_code", 0);
    std::stringstream tmp ;
    tmp << status_code;
    body.insert(find, "Error " + tmp.str());
    body.erase(find + 9, 10);

    find = body.find("reason", find + 4);
    body.insert(find, code_response[status_code]);
    body.erase(find + code_response[status_code].length(), 6);
    send_size = body.length();
    send_flag = false;
    my_response = (char*)body.c_str();
    
}

void    Response::time_out_response(const configfile& conf)
{
    set_status_code(408);
    set_my_response(conf);
    set_body();

    std::string tmp = header + body;
    send_size = tmp.size();
    resp_string = tmp;
    my_response = const_cast<char*>(resp_string.c_str());

    time_out = true;
}

void    Response::set_status_code(int status)
{
    status_code = status;
}

std::string     Response::get_status_line(){return (status_line);}
std::string     Response::get_content_type(){return (content_type);}
std::string     Response::get_content_length(){return (content_length);}
std::string     Response::get_body(){return(body);}
std::string     Response::get_method(){return(method);}
const char*           Response::get_my_response(){return (my_response);}
std::string     Response::get_code_response(int code){return (code_response[code]);}

void            Response::set_request_target(std::string target)
{
    request_target = target;
}
