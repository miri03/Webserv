#include "request.hpp"
#include "response.hpp"

Request::Request()
{
    i = 0;
    method = "none";
    read_fail = false;
    done_reading_header = false;
    req_status_code = -1;
    start.tv_sec = 0;
    start.tv_usec = 0;
    end.tv_usec = 0;
    end.tv_sec = 0;

}

std::string toLower(const std::string& str) 
{
    std::string lowerStr = str;
    for (std::string::iterator it = lowerStr.begin(); it != lowerStr.end(); ++it) {
        *it = std::tolower(*it);
    }
    return lowerStr;
}

bool compareWithString(const std::string& str, const std::map<std::string, std::string>& myMap) 
{
    for (std::map<std::string, std::string>::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
        if (toLower(str) == toLower(it->first)) {
            return true;
        }
    }
    return false;
}

Request::~Request() {}

int char_not_allowed(std::string target)
{
    std::string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    for (int i = 0; target[i]; i++)
    {
        int j = 0;
        for ( ; allowed[j]; j++)
        {
            if (target[i] == allowed[j])
                break;
        }
        if (allowed.length() == (size_t)j)
            return 1;
    }
    return 0;
}

int Request::request_parsing(int fd,configfile &conf)
{
   char buffer_c[1024];
    ssize_t valread = 0;
    std::string buffer;
   if ((method == "none" || method == "POST") && i != 2)
    {
        valread = read(fd, buffer_c, 1024);
        if (valread == 0 || valread == -1)
        {
            read_fail = true;
            return 1;
        }
            
        if ( i == 0)
        {
                hold.append(buffer_c,valread);
                if(hold.rfind("\r\n\r\n")!= std::string::npos)
                    buffer = hold;
                else
                    return 0;
        }        
    }
    if(i == 0)
    {
        size_t endline;
        size_t key;
        size_t value;
        key = buffer.find(' ');
        if (key == std::string::npos)
        {
            i = 2;
            req_status_code = 400;
            done_reading_header = true;
            return 400;
        }
        endline = buffer.find("\r\n");
        if (endline == std::string::npos) 
        {
            req_status_code = 400;
            done_reading_header = true;
            return 400;
        }
        value = endline - key - 1;
        endline += 2;
        method = buffer.substr(0,key);
        tmp = buffer.substr(key+1,value);
        key = tmp.find(' ');
        if (key == std::string::npos) 
        {
            i = 2;
            req_status_code = 400;
            done_reading_header = true;
            return 400;
        
        }
        target = tmp.substr(0,key);
        version = tmp.substr(key+1);
        if (target.size() > 2048)
        {
            i = 2;
            req_status_code = 414;
            done_reading_header = true;
            return 414;
        }    
        if (version != "HTTP/1.1")
        {
            req_status_code = 505;
            done_reading_header = true;
            return 505;
        }
            
        if (char_not_allowed(target))
        {
            req_status_code = 400;
            done_reading_header = true;
            return 400;
        }
        if(method != "GET" && method != "POST" && method != "DELETE")
        {
            if(method == "PUT" || method == "PATCH" || method == "HEAD" || method == "OPTIONS" || method == "TRACE" || method == "CONNECT")
            {
                req_status_code = 501;
                done_reading_header = true;
                return 501;
            }
            req_status_code = 400;
            done_reading_header = true;
            return 400;
        }

        while (i == 0)
        {
            if (endline == buffer.find("\r\n",endline))
            {
                if (method == "GET" || method == "DELETE")
                {
                    i = 1;
                    done_reading_header = true;
                }    
                break;
            }
            key = buffer.find(':',endline) - endline;
            if (key == std::string::npos)
            {
                req_status_code = 400;
                done_reading_header = true;
                return 400;
            }
            value = buffer.find("\r\n",endline) - endline - key - 2;
            if (value == std::string::npos)
            {
                req_status_code = 400;
                done_reading_header = true;
                return 400;
            }
            if (compareWithString(buffer.substr(endline, key),Mymap) == true)
            {
                req_status_code = 400;
                done_reading_header = true;
                return 400;
            }
            std::string string_tmp = toLower(buffer.substr(endline, key));
            Mymap[string_tmp] = buffer.substr(endline +key + 2, value);
            endline = buffer.find("\r\n",endline) + 2;
            if (endline == std::string::npos)
            {
                req_status_code = 400;
                done_reading_header = true;
                return 400;
            }
        }
        size = endline +2;
    }
    else
    {
        if (method == "POST")
            buffer.append(buffer_c,valread);
    }
    if (method == "GET" || method == "DELETE")
    {
        std::map<std::string, std::string>::iterator it;
        it = Mymap.find("host");
        if (it == Mymap.end())
        {
            done_reading_header = true;
            req_status_code = 400;
            return 400;
        }
        post.srv_post = find_server_request(*this, conf);
    }
    if (method == "POST")
    {
        std::map<std::string, std::string>::iterator it;
        it = Mymap.find("transfer-encoding");
        if (it != Mymap.end())
        {
            if(it->second != "chunked")
            {
                done_reading_header = true;
                req_status_code = 501;
                return 501;
            }
        }
    }
    if (method == "POST")
    {
        post.srv_post = find_server_request(*this, conf);
        std::map<std::string, std::string>::iterator it;
        it = Mymap.find("content-length");
        if (it == Mymap.end())
        {
            it = Mymap.find("transfer-encoding");
            if (it == Mymap.end())
            {
                done_reading_header = true;
                req_status_code = 411;
                return 411;
            }
            else if(it->second != "chunked")
            {
                done_reading_header = true;
                req_status_code = 400;
                return 400;
            }
        }
        else
        {
            for (std::string::size_type i = 0; i < it->second.length() ;i++)
            {
                if(isdigit(it->second[i]) == 0)
                {
                    done_reading_header = true;
                    req_status_code = 400;
                    return 400;
                }
            }
            if (std::atoi(it->second.c_str()) == 0)
            {
                it = Mymap.find("transfer-encoding");
                if(it == Mymap.end())
                {
                   done_reading_header = true;
                    req_status_code = 400;
                    return 400; 
                }
            }
        }
    }
    if (method == "POST")
    {
        post.location_post = get_location_post(*this, conf,post.srv_post,&post.index_location);
        if (post.location_post == "404")
        {
            done_reading_header = true;
            req_status_code = 404;
            return 404;
        }
        if (conf.servers[post.srv_post].locations[post.index_location].post == 0)
        {
            done_reading_header = true;
            req_status_code = 405;
            return 405;
        }
        int j = post.tbd(i,size,Mymap,buffer,conf);
        if (j != 0 && j != 1)
        {
            done_reading_header = true;
            req_status_code = j;
            return j;
        }
        if (j == 0)
        {
            done_reading_header = true;
            req_status_code = 201;
            file_posted = post.file_name;
            return 1;
        }
        i = 1;
    }
    return 0;
}

std::string Request::get_target() const
{
    return (target);
}
std::string Request::get_method() const
{
    return (method);
}

std::string Request::get_Mymap(std::string key) const
{
    if ((Mymap.find(key)) == Mymap.end())
        return "\0";
    return ((Mymap.find(key))->second);
}

std::string Request::get_version() const
{
    return (version);
}
