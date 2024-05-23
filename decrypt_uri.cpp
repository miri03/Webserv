/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decrypt_uri.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zbentale <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 15:31:21 by meharit           #+#    #+#             */
/*   Updated: 2024/05/12 11:23:21 by zbentale         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

char hexToChar(const std::string& hex)
{
    std::string test;

    test = hex.substr(0, 2);
    int charCode = std::strtol(test.c_str(), NULL, 16);

    return static_cast<char>(charCode);
}

bool check_if_hexas(std::string str)
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


int decrypt(std::string& target)
{
    std::string tmp;
    
    for(;;)
    {
        if (target[0] == '%'&& check_if_hexas(target.substr(1,2)) == true)
        {
            tmp += hexToChar(&target[1]);
            target.erase(0,3);
        }
        else
            break;
    }
    target = tmp + target;
    return (1);
}

void decrypt_uri(std::string& target)
{
    std::string tmp;
    size_t i = 0;
    
    while (1)
    {
        if (target.find('%',i) == std::string::npos)
            break;
        tmp = target.substr(0, target.find('%',i));
        target.erase(0, target.find('%',i));
        i = tmp.size();
        
        if ( check_if_hexas(target.substr(1,2)) == true)
            decrypt(target);
        else 
            i += 1; 
        target = tmp + target;
    }
}