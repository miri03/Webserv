#pragma once
#include "configfile.hpp"
#include <iostream>
#include <fstream> 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <map>

class Post {
    private:
        std::string body;
        size_t max_read_size;
        size_t max_read; // content-len
        int first;
        size_t size_chunked;
       // size_t body_size;
        std::string size_string;
        std::map<std::string, std::string> types;
        bool help;
        bool help2;
        bool chunked;
        std::string help_string;
        bool flag_lba9i ;
        std::string lba9i;
        size_t chunksize_tmp;
        size_t l;
        bool need_size;
        size_t idk_i;
    public:
        bool content_len_flag;
        std::string location_post;
        int srv_post;
        int index_location;
        std::string file_name;
        std::ofstream aa;
        int tbd(int,int, std::map<std::string, std::string>,std::string,configfile &conf);
        Post();
        ~Post();
};