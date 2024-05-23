/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_resp.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <meharit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 16:13:35 by meharit           #+#    #+#             */
/*   Updated: 2024/05/11 12:24:44 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include "Multiplexing.hpp"

void    Multiple::close_connection(int epollfd, int fd, std::map<int,Request*> &tbd)
{
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0) == -1)
        return ;
    close(fd);
    delete resp[fd];
    delete tbd[fd];
    tbd.erase(fd);
    resp.erase(fd);
}

void    Multiple::resp_send(int fd, std::map<int,Request*> &tbd, int epollfd)
{   
    if ((resp[fd]->cgi1->is_done) || ((resp[fd]->header_flag == true || resp[fd]->read_flag == true) && resp[fd]->send_flag == false))
    {
        signal(SIGPIPE,SIG_IGN);
        int sending = send(fd, resp[fd]->get_my_response(), resp[fd]->send_size, 0);
        if (sending == -1 || sending == 0)
        {
            close_connection(epollfd, fd, tbd);
            return ;
        }
        resp[fd]->read_flag = false;
        resp[fd]->send_flag = true;
        if (resp[fd]->time_out)
        {
            close_connection(epollfd, fd, tbd);
            return ;
        }
    }
    
    if (resp[fd]->done)
        close_connection(epollfd, fd, tbd);
}
