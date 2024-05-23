/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:08:52 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/11 23:07:39 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <sstream>

#include "Multiplexing.hpp"



void Multiple::server(configfile &conf)
{
    int i = 0;
    int size = conf.servers.size();
    int server_fd[size], new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t  addrlen = sizeof(address);
    std::map<int,Request*> tbd;
    
    while(i < size)
    {
        if((server_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            return ;
        }
        if(setsockopt(server_fd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            return ;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = conf.servers[i].server_host;
        address.sin_port = htons(conf.servers[i].port);
        if(bind(server_fd[i], (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            return ;
        }
        if(listen(server_fd[i], SOMAXCONN) < 0)
        {
            perror("listen");
            return ;
        }
        i++;
    }
    struct epoll_event ev, events[1024];
    int nfds, epollfd;
    epollfd = epoll_create(1);
    if(epollfd == -1)
    {
        perror("epoll_create");
        return ;
    }
    for(int i = 0; i < size; i++)
    {
        ev.events = EPOLLIN;
        ev.data.fd = server_fd[i];
        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd[i], &ev) == -1)
        {
            perror("epoll_ctl: server_fd");
            return ;
        }
    }

    while(1)
    {
        
        nfds = epoll_wait(epollfd, events, 1024, -1);
        if(nfds == -1)
        {
            perror("epoll_wait");
                continue;
        }

        for(int n = 0; n < nfds ; n++)
        {
            
            if(events[n].data.fd <= server_fd[size - 1])
            {
                if((new_socket = accept(events[n].data.fd, (struct sockaddr *)&address, &addrlen)) < 0)
                {
                    perror("accept");
                        continue;
                }
                Request *req = new Request();
                gettimeofday(&req->start, NULL);
                tbd[new_socket] = req;
                ev.events = EPOLLIN  | EPOLLOUT | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
                ev.data.fd = new_socket;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, new_socket, &ev) == -1)
                {
                    perror("epoll_ctl: new_socket");
                        continue;
                }
            }
            else
            {
                
                if(events[n].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
                {
                    std::map<int, Response*>::iterator it = resp.find(events[n].data.fd);
                    if (it != resp.end())
                    {
                        // kill cgi if it's running
                        if (resp[events[n].data.fd]->cgi1->is_in == true && resp[events[n].data.fd]->cgi1->is_done == false )
                        {
                            remove(resp[events[n].data.fd]->cgi1->filename.c_str());
                            if(resp[events[n].data.fd]->method == "POST")
                                remove(resp[events[n].data.fd]->cgi1->cgi_file_posted.c_str());
                            kill(resp[events[n].data.fd]->cgi1->pid, SIGKILL);
                            waitpid(resp[events[n].data.fd]->cgi1->pid, NULL, 0);
                        }
                        close_connection(epollfd, events[n].data.fd, tbd);
                        continue;
                    }
                }
                std::map<int, Response*>::iterator it = resp.find(events[n].data.fd);
                if (it == resp.end())
                {
                    resp[events[n].data.fd] = new Response();
                    resp[events[n].data.fd]->fd = events[n].data.fd;
                }
                if(events[n].events & EPOLLIN)
                {
                    //check time
                    if(resp[events[n].data.fd]->first_time == false)
                    {
                        resp[events[n].data.fd]->start = tbd[events[n].data.fd]->start;
                        resp[events[n].data.fd]->first_time = true;
                    }
                    else
                        gettimeofday(&resp[events[n].data.fd]->start, NULL);
                    tbd[events[n].data.fd]->post.srv_post = find_server_request(*tbd[events[n].data.fd], conf);
                    if(tbd[events[n].data.fd]->done_reading_header == false)
                        tbd[events[n].data.fd]->request_parsing(events[n].data.fd, conf);
                }
                if(events[n].events & EPOLLOUT )
                {
                    if (tbd[events[n].data.fd]->read_fail)
                        close_connection(epollfd, events[n].data.fd, tbd);
                    else if (!success(tbd[events[n].data.fd]->req_status_code) && tbd[events[n].data.fd]->req_status_code != -1)
                    {
                        resp[events[n].data.fd]->server_index = tbd[events[n].data.fd]->post.srv_post;
                        gettimeofday(&resp[events[n].data.fd]->start, NULL);
                        resp[events[n].data.fd]->set_status_code(tbd[events[n].data.fd]->req_status_code);
                        if (tbd[events[n].data.fd]->get_method() == "HEAD")
                            resp[events[n].data.fd]->method = "HEAD";
                        resp[events[n].data.fd]->set_my_response(conf);
                        resp_send(events[n].data.fd, tbd, epollfd);
                    }
                    else if(tbd[events[n].data.fd]->done_reading_header == true || resp[events[n].data.fd]->time_out)
                    {
                        gettimeofday(&resp[events[n].data.fd]->start, NULL);
                        if (!resp[events[n].data.fd]->time_out && ((resp[events[n].data.fd]->status_code == 200 || resp[events[n].data.fd]->status_code == 0 ) || (resp[events[n].data.fd]->is_cgi && !server_error(resp[events[n].data.fd]->status_code ))))
                            resp[events[n].data.fd]->methods(*tbd[events[n].data.fd], conf);
                        else if ((resp[events[n].data.fd]->status_code != 200 && !resp[events[n].data.fd]->is_cgi) ||  (resp[events[n].data.fd]->is_cgi && server_error(resp[events[n].data.fd]->status_code )))
                            resp[events[n].data.fd]->set_my_response(conf);
                        resp_send(events[n].data.fd, tbd, epollfd);
                    }                        
                }
                std::map<int, Response*>::iterator it_time = resp.find(events[n].data.fd);
                
                if(it_time != resp.end())
                {
                    if(resp[events[n].data.fd]->first_time == false)
                    {
                        resp[events[n].data.fd]->start = tbd[events[n].data.fd]->start;
                        resp[events[n].data.fd]->first_time = true;
                    }
                    gettimeofday(&resp[events[n].data.fd]->end, NULL);
                    double time = (double)(resp[events[n].data.fd]->end.tv_sec - resp[events[n].data.fd]->start.tv_sec);
                    if(time >= 20 && resp[events[n].data.fd]->cgi1->is_in == false)
                    {
                        resp[events[n].data.fd]->time_out_response(conf);
                    }
                }
            }
        }
    }
}
