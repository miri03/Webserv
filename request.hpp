#pragma once
#include <iostream>
#include <fstream> 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <map>
#include "post.hpp"

class Request {
    private:
        int i;
        size_t size;
        std::string method;
        std::string tmp;
        std::string target;
        std::string version;
        std::string hold;
    public:
        Post post;
        
        Request();
        ~Request();
        bool read_fail;
        std::string file_posted;
        bool done_reading_header;
        int req_status_code;
        std::map<std::string,std::string> Mymap;
        int request_parsing(int,configfile &conf);
        /////////////GETTERS//////////////
        std::string get_method() const;
        std::string get_target() const;
        std::string get_Mymap(std::string) const;
        std::string get_version() const;
        //time out 
        struct timeval start;
        struct timeval end;
        
};

