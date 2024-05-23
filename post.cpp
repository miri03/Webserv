#include "post.hpp"
#include "request.hpp"
#include <cstdlib>
#include <ctime>
#include <dirent.h>
std::string generateRandomFileName() {
    static int j;
    const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string randomName;
    srand(time(NULL));
    for (int i = 0; i < 8; ++i) {
        randomName += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    randomName = randomName + alphanum[j % (sizeof(alphanum) - 1)];
    j++;
    return randomName;
}

bool check_if_hexa(std::string str)
{
    std::string hex = "0123456789ABCDEFabcdef";
    for (size_t i = 0; i < str.size(); i++)
    {
        for (int j = 0; j < 22; j++)
        {
            if(str [i] == hex[j])
                break;
            if (j == 21)
               return false;
        }
    }
    return true;
}

Post::Post()
{
    need_size = false;
    flag_lba9i = false;
    max_read_size = 0;
    first = 1;
    chunked = false;
    help = false;
    help2 = false;
    content_len_flag = false;
    types["audio/aac"] = ".aac";
    types["application/x-abiword"] = ".abw";
    types["image/apng"] = ".apng";
    types["application/x-freearc"] = ".arc";
    types["image/avif"] = ".avif";
    types["video/x-msvideo"] = ".avi";
    types["application/vnd.amazon.ebook"] = ".azw";
    types["application/octet-stream"] = ".bin";
    types["image/bmp"] = ".bmp";
    types["application/x-bzip"] = ".bz";
    types["application/x-bzip2"] = ".bz2";
    types["application/x-cdf"] = ".cda";
    types["application/x-csh"] = ".csh";
    types["text/css"] = ".css";
    types["text/csv"] = ".csv";
    types["application/msword"] = ".doc";
    types["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    types["application/vnd.ms-fontobject"] = ".eot";
    types["application/epub+zip"] = ".epub";
    types["application/gzip"] = ".gz";
    types["image/gif"] = ".gif";
    types["text/html"] = ".htm";
    types["text/html"] = ".html";
    types["image/vnd.microsoft.icon"] = ".ico";
    types["text/calendar"] = ".ics";
    types["application/java-archive"] = ".jar";
    types["image/jpeg"] = ".jpeg";
    types["text/javascript"] = ".js";
    types["application/json"] = ".json";
    types["application/ld+json"] = ".jsonld";
    types["audio/midi"] = ".midi";
    types["text/javascript"] = ".mjs";
    types["audio/mpeg"] = ".mp3";
    types["video/mp4"] = ".mp4";
    types["video/mpeg"] = ".mpeg";
    types["application/vnd.apple.installer+xml"] = ".mpkg";
    types["application/vnd.oasis.opendocument.presentation"] = ".odp";
    types["application/vnd.oasis.opendocument.spreadsheet"] = ".ods"; 
    types["application/vnd.oasis.opendocument.text"] = ".odt";
    types["audio/ogg"] = ".oga";
    types["video/ogg"] = ".ogv";
    types["application/ogg"] = ".ogx";
    types["audio/opus"] = ".opus";
    types["font/otf"] = ".otf";
    types["image/png"] = ".png";
    types["application/pdf"] = ".pdf";
    types["application/x-httpd-php"] = ".php";
    types["application/vnd.ms-powerpoint"] = ".ppt";
    types["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
    types["application/vnd.rar"] = ".rar";
    types["application/rtf"] = ".rtf";
    types["application/x-sh"] = ".sh";
    types["image/svg+xml"] = ".svg";
    types["application/x-tar"] = ".tar";
    types["image/tiff"] = ".tiff";
    types["video/mp2t"] = ".ts";
    types["font/ttf"] = ".ttf";
    types["text/plain"] = ".txt";
    types["application/vnd.visio"] = ".vsd";
    types["audio/wav"] = ".wav";
    types["audio/webm"] = ".weba";
    types["video/webm"] = ".webm";
    types["image/webp"] = ".webp";
    types["font/woff"] = ".woff";
    types["font/woff2"] = ".woff2";
    types["application/xhtml+xml"] = ".xhtml";
    types["application/vnd.ms-excel"] = ".xls";
    types["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    types["application/xml"] = ".xml";
    types["application/vnd.mozilla.xul+xml"] = ".xul";
    types["application/zip"] = ".zip";
    types["video/3gpp"] = ".3gp";
    types["video/3gpp2"] = ".3g2";
    types["application/x-7z-compressed"] = ".7z";
    types["application/x-www-form-urlencoded"] = ".txt";
}


Post::~Post()
{    
}

bool if_cgi(std::string str)
{
    unsigned long i = str.find("/cgi/");
    if(i != std::string::npos)
        return true;
    return false;
}

int Post::tbd(int i,int size,std::map<std::string, std::string> Mymap,std::string buffer,configfile &conf)
{
    if (first == 1)
    {
        std::string value = Mymap["content-length"];
        char *endptr;
        max_read = std::strtoul(value.c_str(), &endptr, 10);
        std::map<std::string, std::string>::iterator it4 = Mymap.find("content-length");
        if (it4 != Mymap.end())
            content_len_flag = true;
        std::map<std::string, std::string>::iterator it3 = Mymap.find("transfer-encoding");
        if (it3 != Mymap.end())
            chunked = true;
        else
            first = 0;
    }
    if (chunked == true)
    {
        content_len_flag = false;
        std::string tmp;
        if (i == 0)     
            tmp = buffer.substr(size,buffer.length());
        else
            tmp.append(buffer);
        if ( first == 1)
        {
            file_name = generateRandomFileName();
            std::map<std::string, std::string>::iterator it = Mymap.find("content-type");
            if(it == Mymap.end())
                return 400;
            std::map<std::string, std::string>::const_iterator it2 = types.find(it->second);
            if (it2 != types.end())
                file_name += it2->second;
            else if(if_cgi(location_post) == true)
                file_name += ".txt";
            else
                return 415;
            if (conf.servers[srv_post].locations[index_location].upload == true)
            {
                DIR* directory = opendir(conf.servers[srv_post].locations[index_location].upload_path.c_str());
                if (directory == NULL)
                    return 500;
                if (access(conf.servers[srv_post].locations[index_location].upload_path.c_str(), W_OK) != 0)
                {
                    closedir(directory);
                    return 500;
                }
                closedir(directory);
                int last_index = conf.servers[srv_post].locations[index_location].upload_path.length() - 1;
                if (conf.servers[srv_post].locations[index_location].upload_path[last_index] != '/')
                    file_name = conf.servers[srv_post].locations[index_location].upload_path + '/' + file_name;
                else
                    file_name = conf.servers[srv_post].locations[index_location].upload_path + file_name;
            }
            else 
                return 403;
            first = 0;
            aa.open(file_name.c_str(), std::ios::app);    
        }
        idk_i = 0;
        while (idk_i<tmp.size())
        {
            if (need_size == false)
            {
                if (flag_lba9i == true)
                {
                    tmp = lba9i + tmp;
                    flag_lba9i = false;
                }
                if (tmp.find("\r\n",idk_i) == std::string::npos)
                {
                    flag_lba9i = true;
                    lba9i = tmp.substr(idk_i);
                    break;
                }
                else
                {
                    if (tmp.find("\r\n",idk_i) == 0)
                        idk_i += 2;
                    size_string = tmp.substr(idk_i,tmp.find("\r\n",idk_i) - idk_i);
                    if (check_if_hexa(size_string) == false)
                    {
                        std::remove(file_name.c_str());
                        return 400;
                    }
                    const char* c_string = size_string.c_str();
                    char* end_ptr;
                    size_chunked = strtoul(c_string,&end_ptr,16);
                    chunksize_tmp = size_chunked;
                    if (size_chunked == 0)
                    {
                        aa.close();
                        return 0;
                    }
                    need_size = true;
                    l = 0;
                    tmp = tmp.substr(tmp.find("\r\n",idk_i) + 2);
                }
            }
            l += tmp.size();
            if (l < size_chunked)
            {
                if (aa.is_open())
                {
                    aa << tmp;
                    chunksize_tmp -= tmp.size();
                    max_read_size += tmp.size();
                }
                else if (aa.is_open() == false)
                    return 500;
                if (conf.servers[srv_post].client_max_body_size_set == true && max_read_size > conf.servers[srv_post].client_max_body_size)
                {
                    std::remove(file_name.c_str());
                    return 413;
                }
                break;
            }        
            else
            {
                idk_i = chunksize_tmp;
                if (aa.is_open())
                {
                    aa << tmp.substr(0,idk_i);
                    max_read_size += tmp.substr(0,idk_i).size();
                }
                else if (aa.is_open() ==  false)
                    return 500;
                if (conf.servers[srv_post].client_max_body_size_set == true && max_read_size > conf.servers[srv_post].client_max_body_size)
                {
                    std::remove(file_name.c_str());
                    return 413;
                }
                idk_i += 2;
                if (idk_i == tmp.size()+1)
                {
                    flag_lba9i = true;
                    lba9i = tmp.substr(idk_i - 2);
                    need_size = false;
                    break;
                }
                need_size = false;
                continue;
            }
        }
    }
    else
    {
        if (i != 0)
        {
            if (aa.is_open())
            {
                if (max_read_size + buffer.size() > max_read)
                {
                    aa.write(buffer.c_str() , max_read - max_read_size);
                    aa.flush();
                    aa.close();
                    return 0;
                }
                aa.write(buffer.c_str() , buffer.size());
                max_read_size += buffer.size();
                if (conf.servers[srv_post].client_max_body_size_set == true && max_read_size > conf.servers[srv_post].client_max_body_size)
                {
                    std::remove(file_name.c_str());
                    return 413;
                }
                buffer.clear();
            }
            else if (aa.is_open() == false)
                return 500 ;
        }
        else
        {   
            file_name = generateRandomFileName();
            std::map<std::string, std::string>::iterator it = Mymap.find("content-type");
            if(it == Mymap.end())
                return 400;
            std::map<std::string, std::string>::const_iterator it2 = types.find(it->second);
             if (it2 != types.end())
                file_name += it2->second;
            else if(if_cgi(location_post) == true)
                file_name += ".txt";
            else
                return 415;
            if (conf.servers[srv_post].locations[index_location].upload == true)
            {
                DIR* directory = opendir(conf.servers[srv_post].locations[index_location].upload_path.c_str());
                if (directory == NULL)
                    return 500;
                if (access(conf.servers[srv_post].locations[index_location].upload_path.c_str(), W_OK) != 0)
                {
                    closedir(directory);
                    return 500;
                }
                closedir(directory);
                int last_index = conf.servers[srv_post].locations[index_location].upload_path.length() - 1;
                if (conf.servers[srv_post].locations[index_location].upload_path[last_index] != '/')
                    file_name = conf.servers[srv_post].locations[index_location].upload_path + '/' + file_name;
                else
                    file_name = conf.servers[srv_post].locations[index_location].upload_path + file_name;
            }
            else 
                return 403;
            aa.open(file_name.c_str(), std::ios::app);
            if (aa.is_open())
            {
                if (buffer.substr(size,buffer.length()).length() > max_read)
                {
                    aa << buffer.substr(size, max_read);
                    aa.flush();
                    aa.close();
                    return 0;
                }
                aa << buffer.substr(size,buffer.length());
                max_read_size += buffer.substr(size,buffer.length()).size();
                if (conf.servers[srv_post].client_max_body_size_set == true && max_read_size > conf.servers[srv_post].client_max_body_size)
                {
                    std::remove(file_name.c_str());
                    return 413;
                }
            }
            else if (aa.is_open() == false)
                return 500 ;
        }
        if (max_read_size == max_read)
        {
            aa.flush();
            aa.close();
            return 0;
        }
    }
    return 1;
}