/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_reason.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <meharit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:48:03 by meharit           #+#    #+#             */
/*   Updated: 2024/05/09 19:37:55 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

void    Response::code_reason()
{
    code_response[100] = "continue";//
    code_response[101] = "Switching Protocols";//
    code_response[200] = "OK";
    code_response[201] = "Created";
    code_response[202] = "Accepted";//
    code_response[203] = "Non-Authoritative Information";//
    code_response[204] = "No Content";
    code_response[205] = "Reset Content";//
    code_response[206] = "Partial Content";//
    code_response[300] = "Multiple Choices";//
    code_response[301] = "Moved Permanently";
    code_response[302] = "Found";//
    code_response[303] = "See Other";//
    code_response[304] = "Not Modified";//
    code_response[305] = "Use Proxy";//
    code_response[307] = "Temporary Redirect";//
    code_response[400] = "Bad Request";
    code_response[401] = "Unauthorized";//
    code_response[402] = "Payment Required";//
    code_response[403] = "Forbidden";
    code_response[404] = "Not Found";
    code_response[405] = "Method Not Allowed";
    code_response[406] = "Not Acceptable";//
    code_response[407] = "Proxy Authentication Required";//
    code_response[408] = "Request Time-out";
    code_response[409] = "Conflict";
    code_response[410] = "Gone";//
    code_response[411] = "Length Required";
    code_response[412] = "Precondition Failed";//
    code_response[413] = "Request Entity Too Large";
    code_response[414] = "Request-URI Too Large";
    code_response[415] = "Unsupported Media Type";
    code_response[416] = "Requested range not satisfiable";//
    code_response[417] = "Expectation Failed";//
    code_response[500] = "Internal Server Error";
    code_response[501] = "Not Implemented";
    code_response[502] = "Bad Gateway";//
    code_response[503] = "Service Unavailable";//
    code_response[504] = "Gateway Time-out";
    code_response[505] = "HTTP Version not supported";
}

void    Response::mime_map()
{
    mimes[".aac"] = "audio/aac";
    mimes[".abw"] = "application/x-abiword";
    mimes[".apng"] = "image/apng";
    mimes[".arc"] = "application/x-freearc";
    mimes[".avif"] = "image/avif";
    mimes[".avi"] = "video/x-msvideo";
    mimes[".azw"] = "application/vnd.amazon.ebook";
    mimes[".bin"] = "application/octet-stream";
    mimes[".bmp"] = "image/bmp";
    mimes[".bz"] = "application/x-bzip";
    mimes[".bz2"] = "application/x-bzip2";
    mimes[".cda"] = "application/x-cdf";
    mimes[".csh"] = "application/x-csh";
    mimes[".css"] = "text/css";
    mimes[".csv"] = "text/csv";
    mimes[".doc"] = "application/msword";
    mimes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimes[".eot"] = "application/vnd.ms-fontobject";
    mimes[".epub"] = "application/epub+zip";
    mimes[".gz"] = "application/gzip";
    mimes[".gif"] = "image/gif";
    mimes[".htm"] = "text/html";
    mimes[".html"] = "text/html";
    mimes[".ico"] = "image/vnd.microsoft.icon";
    mimes[".ics"] = "text/calendar";
    mimes[".jar"] = "application/java-archive";
    mimes[".jpeg"] = "image/jpeg";
    mimes[".jpg"] = "image/jpeg";
    mimes[".js"] = "text/javascript";
    mimes[".json"] = "application/json";
    mimes[".jsonld"] = "application/ld+json";
    mimes[".mid"] = "audio/midi";
    mimes[".midi"] = "audio/midi";
    mimes[".mjs"] = "text/javascript";
    mimes[".mp3"] = "audio/mpeg";
    mimes[".mp4"] = "video/mp4";
    mimes[".mpeg"] = "video/mpeg";
    mimes[".mpkg"] = "application/vnd.apple.installer+xml";
    mimes[".odp"] = "application/vnd.oasis.opendocument.presentation";
    mimes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    mimes[".odt"] = "application/vnd.oasis.opendocument.text";
    mimes[".oga"] = "audio/ogg";
    mimes[".ogv"] = "video/ogg";
    mimes[".ogx"] = "application/ogg";
    mimes[".opus"] = "audio/opus";
    mimes[".otf"] = "font/otf";
    mimes[".png"] = "image/png";
    mimes[".pdf"] = "application/pdf";
    mimes[".php"] = "application/x-httpd-php";
    mimes[".ppt"] = "application/vnd.ms-powerpoint";
    mimes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimes[".rar"] = "application/vnd.rar";
    mimes[".rtf"] = "application/rtf";
    mimes[".sh"] = "application/x-sh";
    mimes[".svg"] = "image/svg+xml";
    mimes[".tar"] = "application/x-tar";
    mimes[".tif"] = "image/tiff";
    mimes[".tiff"] = "image/tiff";
    mimes[".ts"] = "video/mp2t";
    mimes[".ttf"] = "font/ttf";
    mimes[".txt"] = "text/plain";
    mimes[".vsd"] = "application/vnd.visio";
    mimes[".wav"] = "audio/wav";
    mimes[".weba"] = "audio/webm";
    mimes[".webm"] = "video/webm";
    mimes[".webp"] = "image/webp";
    mimes[".woff"] = "font/woff";
    mimes[".woff2"] = "font/woff2";
    mimes[".xhtml"] = "application/xhtml+xml";
    mimes[".xls"] = "application/vnd.ms-excel";
    mimes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimes[".xml"] = "application/xml";
    mimes[".xul"] = "application/vnd.mozilla.xul+xml";
    mimes[".zip"] = "application/zip";
    mimes[".3gp"] = "video/3gpp";
    mimes[".3g2"] = "video/3gpp2";
    mimes[".7z"] = "application/x-7z-compressed";
    mimes[".cpp"] = "text/plain";
    mimes[".hpp"] = "text/plain";
    mimes[".conf"] = "text/plain";
}