/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:08 by viroques          #+#    #+#             */
/*   Updated: 2022/01/27 11:20:40 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
#define RESPONSE_H

# include <iostream>
# include <fstream>
# include <sstream>

# include <map>


// PathIsfile Include
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>

# include "Request_Parser.hpp"
# include "cgiHandler.hpp"
# include "Server.hpp"
# include "autoIndex.hpp"

class Response {

public:

    Response(Request &req, Server &serv);
    Response();

    std::string&    call();
    int             readHtml(std::string &path);
    void            readContent(std::string &path);
    std::string     writeHeader(std::string status, std::string contentType, size_t bodyLength);
    std::string&    getMethod();
    std::string&    postMethod();
    std::string&    deleteMethod();
    void            setCgiHeader(std::string cgiHeader);
    void            setCgiPath();

    // Utils
    bool            isAllow(std::string method);
    int             pathIsFile(std::string &path);
    int             pathIsDirectory(std::string &path);
    std::string     sizeToString(size_t size);


private:

    Request                     _req;
    std::string                 _status;
    std::string                 _contentType;
    std::string                 _bodyLength;
    std::string                 _path;
    std::string                 _body;
    std::string                 _header;
    std::string                 _response;
    Server                      _serv;
    map_str                     _errorPage;
    std::string                 _pathCgi;
    std::string                 _extensionCgi;
    std::string                 _index;
    std::string                 _root;
    bool                        _AutoIndex;
    vec_str                     _allowMethods;
};

#endif
