/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:08 by viroques          #+#    #+#             */
/*   Updated: 2022/02/17 17:15:33 by viroques         ###   ########.fr       */
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
    
    Response();
    Response(Server &serv);
    Response(Server &serv, Request &req);
    Response(const Response &res);
    ~Response();
    Response    &operator=(const Response &res);

    int             initRequest(Request &req);
    int             searchFd(Request &req);
    void            answer();
    void            setLocationConf();

    /* HTTP Methods */
    void            getMethod();
    void            postMethod();
    void            deleteMethod();
    
    /* Helper Functions */
    bool            isAllow(std::string method);
    int             upload();
    std::string		autoIndexBuilder(std::string path);
    int             needCgi();

    /* Set Fd */
    void            setFdContent();
    void            setFdError(int code);

    /* Getters */

    Request&        getRequest();
    std::string&    getPath();
    std::string&    getBody();
    std::string&    getResponse();
    Server&         getServ();
    map_str&        getErrorPage();
    std::string&    getPathCgi();
    std::string&    getExtensionCgi();
    std::string&    getIndex();
    std::string&    getRoot();
    std::string&    getGeneralRoot();
    bool            getAutoIndex();
    vec_str&        getAllowMethods();
    std::string&    getUploadDest();
    ResponseHeader& getResponseHeader();
    std::string&    getLocation();

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
    bool                        _autoIndex;
    vec_str                     _allowMethods;
    std::string                 _uploadDest;
    ResponseHeader              _header;
    std::string                 _location;
    int                         _code;
    int                         _fd;
    
    std::map<std::string, void (Response::*)()> _methods;  
};

#endif
