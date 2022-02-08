/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:08 by viroques          #+#    #+#             */
/*   Updated: 2022/02/08 20:08:54 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
#define RESPONSE_H

# include <iostream>
# include <fstream>
# include <sstream>

# include <map>

# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>

# include "ResponseHeader.hpp"  
# include "RequestParser.hpp"
# include "cgiHandler.hpp"
# include "Server.hpp"
# include "autoIndex.hpp"
# include "utils.hpp"

class Response {

public:

    Response(Server &serv);

    void            makeAnswer(Request &req);
    int             initRequest(Request &req);
    int             readErrorPage(std::string &path);
    void            readContent(std::string &path);
    void            getMethod();
    void            headMethod();
    void            postMethod();
    void            deleteMethod();
    int             upload();
    void            setLocationConf();
    void            sendPage(int code);
    std::string&    getResponse();
    
    // Utils
    bool            isAllow(std::string method);
	std::string		autoIndexBuilder(std::string path);

private:

    Request                     _request;
    std::string                 _path;
    std::string                 _body;
    std::string                 _response;
    Server                      _serv;
    map_str                     _errorPage;
    std::string                 _pathCgi;
    std::string                 _extensionCgi;
    std::string                 _index;
    std::string                 _root;
    std::string                 _generalRoot;
    bool                        _AutoIndex;
    vec_str                     _allowMethods;
    std::string                 _uploadDest;
    ResponseHeader              _header;
    std::string                 _location;
    std::string                 _date;

    std::map<std::string, void (Response::*)()> _methods;  
};

#endif
