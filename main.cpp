/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <meharit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/29 15:57:01 by zbentale          #+#    #+#             */
/*   Updated: 2024/05/07 00:08:19 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configfile.hpp"
#include "Multiplexing.hpp"

int main(int argc,char **argv)
{
    if(argc >= 1 && argc <= 2)
    {
        std::ifstream a;
        configfile conf;
        try
        {
            Multiple m;
            std::string default_conf = "default.conf";
            if(argc == 1)
                conf.pars_configfile((char *)default_conf.c_str(),a);
            else
            conf.pars_configfile(argv[1],a);
            m.server(conf);
            return 1;
        }
        catch(const std::exception& e)
        {
            const std::string s = e.what();
            if(s == "duplicat server conf")
            {
                std::cerr << "duplicat server conf" << std::endl;
            }
            else if(conf.line_number != 0)
                std::cerr << "---> Error in line " << conf.line_number << " , " << e.what() << std::endl;
            else
                std::cerr << e.what() << std::endl;
            a.close();
            return 1;
        }
    }
    else if(argc > 2)
    {
        std::cerr << "too many arguments" << std::endl;
        return 1;
    }
        
}