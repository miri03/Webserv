/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:42:23 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/11 18:48:03 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"


void cgi::set_env(const configfile& conf, Response& res)
{
    (void)conf;
    if(res.get_method() == "POST")
        cgi_env["CONTENT_LENGTH"] = res.get_content_length().erase(res.get_content_length().size() - 2);
    cgi_env["CONTENT_TYPE"] = res.get_content_type().erase(res.get_content_type().size() - 2);
    cgi_env["QUERY_STRING"] = res.Query_String;
    cgi_env["REQUEST_METHOD"] = res.method;
    cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    cgi_env["PATH_INFO"] = res.request_target;
    cgi_env["SERVER_PROTOCOL"] = res.version;
    cgi_env["SCRIPT_NAME"] = res.requested_resource;
    cgi_env["SCRIPT_FILENAME"] = res.requested_resource;
    cgi_env["PATH_TRANSLATED"] = res.requested_resource;
    cgi_env["REDIRECT_STATUS"] = "200";
    cgi_env["SERVER_NAME"] = "webserv";
    if(cookie != "")
        cgi_env["HTTP_COOKIE"] = cookie;
    env = new char*[cgi_env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = cgi_env.begin(); it != cgi_env.end(); it++)
    {
        std::string tmp = it->first + "=" + it->second;
        char *tmp2 = new char[tmp.length() + 1];
        strcpy(tmp2, tmp.c_str());
        env[i] = tmp2;
        i++;
    }
    env[i] = NULL;
}

void cgi::exec_cgi(const configfile& conf, Response& res)
{
    extension = res.extension_cgi(res.requested_resource);
    if(extension == ".py" || extension == ".php")
    {
        if(check_if_i_have_it(conf,res,extension) == 1)
        {
            if(extension == ".php" || extension == ".py")
            {
                if(is_in == false)
                {
                    char *cmd_argv[3];
                    status = 0;
                    set_env(conf, res);
                    if(access(path.c_str(), X_OK) == -1)
                    {
                        cgi_status = 500;
                        is_done = true;
                        return;
                    }
                    cmd_argv[0] = const_cast<char*>(path.c_str());
                    cmd_argv[1] = const_cast<char*>(res.requested_resource.c_str());
                    cmd_argv[2] = NULL;
                    std::string home = getenv("HOME");
                    filename = home + "/.cache/tmp/" + generateRandomFileName();
                    std::string d = home + "/.cache/tmp/";
                    DIR *dir = opendir(d.c_str());
                    if(dir == NULL)
                    {
                        if(mkdir(d.c_str(), 0777) == -1)
                        {
                             cgi_status = 500;
                             is_done = true;
                            return;
                        }
                    }
                    gettimeofday(&start, NULL);
                    pid = fork();
                    if(pid == 0)
                    {
                        freopen("/dev/null", "w", stderr);
                        file = freopen(filename.c_str(), "w", stdout);
                        if (file == NULL)
                        {
                            cgi_status = 500;
                            is_done = true;
                            return;
                        }
                        if(res.method == "POST")
                        {
                            file =freopen(cgi_file_posted.c_str(), "r", stdin);
                            if (file == NULL)
                            {
                                cgi_status = 500;
                                is_done = true;
                                return;
                            }
                        }
                        execve(cmd_argv[0], cmd_argv, env);
                        kill(0, SIGKILL);
                    }
                    else if(pid < 0)
                    {
                        cgi_status = 500;
                        is_done = true;
                        return;
                    }
                    else
                    {
                        is_in = true;
                        closedir(dir);
                    }
                }
                else
                {
                        gettimeofday(&end, NULL);
                        if (waitpid(pid, &status, WNOHANG) > 0)
                        {
                            if(WIFEXITED(status))
                            {                                
                                if(extension == ".php")
                                {
                                    is_done = true;
                                    if(parsing_file_cgi())
                                        cgi_status = 500;
                                }
                                else if(extension == ".py")
                                {
                                    if(WEXITSTATUS(status) == 0)
                                    {
                                        is_done = true;
                                    }
                                    else
                                    {
                                        is_done = true;
                                        cgi_status = 500;
                                    }
                                }
                            }
                        }
                        else if(WIFSIGNALED(status))
                        {
                             is_done = true;
                             cgi_status = 500;
                        }
                        else
                        {
                            double  diff = (end.tv_sec - start.tv_sec);
                            if(diff >= 15)
                            {
                                kill(pid, SIGKILL);
                                waitpid(pid, &status, 0);
                                is_done = true;
                                cgi_status = 504;
                            }
                        } 
                }   

            }
            
        }
        else
        {
            cgi_status = 501;
            is_done = true;
            return;
        }
    }
    else
    {
        cgi_status = 501;
        is_done = true;
        return;
    } 
}

int cgi::check_if_i_have_it(const configfile& conf, Response& res, std::string extension)
{
    size_t i = 0;
    for (; i < conf.servers[res.server_index].locations.size() ; i++)
    {
        if (conf.servers[res.server_index].locations[i].name == "/cgi")
            break;
    }
    for(std::map<std::string, std::string>::const_iterator it = conf.servers[res.server_index].locations[i].cgi_conf.begin(); it != conf.servers[res.server_index].locations[i].cgi_conf.end(); it++)
    {
        std::string tmp = "." + it->first;
        if(tmp == extension)
        {
            path = it->second;
            return 1;
        }
    }
    return 0;
}

std::string cgi::generateRandomFileName() 
{
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string randomName;
    srand(time(0));
    for (int i = 0; i < 10; ++i) {
        randomName += alphabet[rand() % alphabet.length()];
    }
    return randomName;
}
int cgi::parsing_file_cgi()
{
    std::string line;
    std::ifstream myfile(filename.c_str());
    int i = 0;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if(line.find("Status:") != std::string::npos && i == 0)
            {
                std::string tmp = line.substr(7);
                cgi_status = atoi(tmp.c_str());
            }
            else if(line.find("Content-type:") != std::string::npos )
            {
                line = line.substr(14);
                size_t found = line.find(";");
                if(found != std::string::npos)
                    line = line.substr(0, found);
                cgi_content_type= line;
                break;
            }
            i++;
            
        }
        myfile.close();
    }
    else
        return 1;

        
    return 0;
}


cgi::~cgi()
{
    
    int size = cgi_env.size();
    for (int i = 0; i < size; i++) 
    {
        delete[] env[i];
    }
        
        delete[] env;
}

