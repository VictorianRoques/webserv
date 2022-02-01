/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/01 16:39:36 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response(Request &req, Server &serv): _req(req), _serv(serv), _errorPage(serv.getErrorPage())
{
    _status = "200 OK";
    _path = req.getFullPath();
    _contentType = req.getContentType();
    setCgiPath();
    if (req.getPath() == "/")
    {
        if (_AutoIndex == true)
            _path.erase(_path.size() - 1);
        else
            _path = _root + "/" + _index;
        _contentType = "text/html";
    }
}

void        Response::setCgiPath()
{
    std::vector<Location *> loc = _serv.getLocation();
    std::vector<Location *>::iterator it = loc.begin();
    size_t pos = _path.length();
	
	while (pos != 0) {
		pos = _path.rfind("/", pos - 1);
		for (; it != loc.end(); it++) {
			if (!strncmp(_path.c_str(), (*it)->getLocationMatch().c_str(), pos)) {
				break ;
			}
			if ((*it)->getLocationMatch() == "/")
            {
                _pathCgi = (*it)->getCgiHandler().second;
                _extensionCgi = (*it)->getCgiHandler().first;
                _index = (*it)->getIndex();
                _root = (*it)->getRoot();
                _AutoIndex = (*it)->getAutoIndex();
                _allowMethods = (*it)->getMethods();
            }
		}
	}
}

int      Response::readHtml(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;

    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
        {
            _body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
            return (1); 
        }
        buffer << ofs.rdbuf();
        ofs.close();
        _body = buffer.str();
        _header = writeHeader("200 OK", "text/html", buffer.str().length());
        return (0);
    }
    else
    {
        _body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
        return (1);
    }
}

void     Response::readContent(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;

 //   std::cout << "PATH: " << path << std::endl;
    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
        {
            if (readHtml(_errorPage["403"]))
                _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
            return ;
        }
        buffer << ofs.rdbuf();
        ofs.close();
        _header =  writeHeader("200 OK", _contentType, buffer.str().length());
        _body = buffer.str();
        return ;
    }
    if (readHtml(_errorPage["404"]))
        _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
}

std::string     Response::writeHeader(std::string status, std::string contentType, size_t bodyLength)
{
    std::string header;

    header = "HTTP/1.1 " + status + "\r\n";
    header += "Content-Type: " + contentType + "\r\n";
    header += "Content-Length: " + sizeToString(bodyLength) + "\r\n\r\n";
    return header;
}


void    Response::setCgiHeader(std::string cgiHeader)
{
    size_t pos = cgiHeader.find("Status: ") + 8;
    if (pos)
        _status = cgiHeader.substr(pos, cgiHeader.find("\r\n", pos));
    pos = cgiHeader.find("Content-type: ") + 14;
    _contentType = cgiHeader.substr(pos, cgiHeader.find("\r\n", pos));
}

std::string&     Response::postMethod()
{
    cgiHandler cgi(_req);
    _body = cgi.execute(_pathCgi);
    if (_body == "<html><body>FATAL ERROR CGI</body></html>")
    {
        _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        _response = _header + _body;
        return _response;
    }
    setCgiHeader(_body.substr(0, _body.find("\r\n\r\n")));      
    _body = _body.substr(_body.find("\r\n\r\n") + 4);
    _header = writeHeader(_status, _contentType, _body.length());
    _response = _header + _body;
    return _response;
}

std::string&     Response::getMethod()
{
    if (_req.getFullPath().find(_extensionCgi) != std::string::npos)
    {
        cgiHandler cgi(_req);
        _body = cgi.execute(_pathCgi);
        if (_body == "<html><body>FATAL ERROR CGI</body></html>")
        {
            _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
            _response = _header + _body;
            return _response;
        }
        setCgiHeader(_body.substr(0, _body.find("\r\n\r\n")));      
        _body = _body.substr(_body.find("\r\n\r\n") + 4);
        _header = writeHeader(_status, _contentType, _body.length());
    }
    else if (_AutoIndex == true && pathIsDirectory(_path))
    {
        _body = autoIndexBuilder(_path);
        _header = writeHeader("200 OK", "text/html", _body.length());
    }
    else
    {
        readContent(_path);
    }
    _response = _header + _body;
//    std::cout << "RES: \n" << _response << "\nEND\n";
    return _response;
}

std::string&     Response::deleteMethod()
{
    _body = "";
    if (pathIsFile(_path) || pathIsDirectory(_path))
    {
        if (remove(_path.c_str()))
        {
            if (readHtml(_errorPage["403"]))
                _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        }
        _header = "HTTP/1.1 204\r\n\r\n";
    }
    else
    {
        if (readHtml(_errorPage["404"]))
            _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
    }
    _response = _header + _body;
    return _response; 
}

bool             Response::isAllow(std::string method)
{
    vec_str::iterator it = _allowMethods.begin();
    vec_str::iterator ite = _allowMethods.end();
    for (; it != ite; it++)
    {
        if (*it == method)
            return true;
    }
    return false;
}

std::string&     Response::call()
{
    if (_req.getBody().size() + 2 > _serv.getMaxBodySize())
    {
        if (readHtml(_errorPage["413"]))
            _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        _response = _header + _body;
        return _response;
    }
    if (_req.getMethod() == "GET" && isAllow("GET"))
        return getMethod();
    else if (_req.getMethod() == "POST" && isAllow("POST"))
        return postMethod();
    else if (_req.getMethod() == "DELETE" && isAllow("DELETE"))
        return deleteMethod();
    else
    {
        if (readHtml(_errorPage["405"]))
             _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        _response = _header + _body;
        return _response;
    }
}

// Utils

std::string Response::sizeToString(size_t size)
{
    std::ostringstream convert;
    convert << size;
    return convert.str();
}

int     Response::pathIsFile(std::string &path)
{
    struct stat s;

	if (stat(path.c_str(), &s) == 0)
        if (s.st_mode & S_IFREG)
			return 1;
	return 0;
}

int     Response::pathIsDirectory(std::string &path)
{
    struct stat s;

	if (stat(path.c_str(), &s) == 0)
        if (s.st_mode & S_IFDIR)
			return 1;
	return 0;
}
