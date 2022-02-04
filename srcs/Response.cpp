/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/03 17:38:48 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response(Server &serv): _serv(serv), _errorPage(serv.getErrorPage()) {}

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

int      Response::readErrorPage(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;

    _header = "";
    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
        {
            _body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
            _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
            return (1);
        }
        buffer << ofs.rdbuf();
        ofs.close();
        _body = buffer.str();
        return (0);
    }
    else
    {
        _body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
        _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        return (1);
    }
}

void     Response::readContent(std::string &path)
{
    std::ifstream       fd;
    std::stringstream   buffer;

    if (pathIsFile(path))
    {
        fd.open(path.c_str(), std::ifstream::in);
        if (fd.is_open() == false)
        {
            if (!readErrorPage(_errorPage["403"]))
                _header = writeHeader("403 Forbidden", "text/html", _body.length());
            return ;
        }
        buffer << fd.rdbuf();
        fd.close();
        _header =  writeHeader("200 OK", _contentType, buffer.str().length());
        _body = buffer.str();
        return ;
    }
    else
    {
        if (!readErrorPage(_errorPage["404"]))
            _header = writeHeader("404 Not Found", "text/html", _body.length());
    }
}

std::string     Response::writeHeader(std::string status, std::string contentType, size_t bodyLength)
{
    std::string header;

    header = "HTTP/1.1 " + status + "\r\n";
    if (contentType.empty() == false)
        header += "Content-Type: " + contentType + "\r\n";
    if (bodyLength > 0)
        header += "Content-Length: " + sizeToString(bodyLength) + "\r\n";
    header += "\r\n";
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

void        Response::getMethod()
{
    if (_req.getFullPath().find(_extensionCgi) != std::string::npos)
    {
        cgiHandler cgi(_req);
        _body = cgi.execute(_pathCgi);
        if (_body == "<html><body>FATAL ERROR CGI</body></html>")
        {
            if (!readErrorPage(_errorPage["500"]))
                _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
            _response = _header + _body;
            return ;
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
}

void     Response::postMethod()
{
    cgiHandler cgi(_req);
    _body = cgi.execute(_pathCgi);
    if (_body == "<html><body>FATAL ERROR CGI</body></html>")
    {
        if (!readErrorPage(_errorPage["500"]))
            _header = writeHeader("500 Internal Servor Error", "text/html", _body.length());
        _response = _header + _body;
        return ;
    }
    setCgiHeader(_body.substr(0, _body.find("\r\n\r\n")));      
    _body = _body.substr(_body.find("\r\n\r\n") + 4);
    _header = writeHeader(_status, _contentType, _body.length());
    _response = _header + _body;
}

void        Response::deleteMethod()
{
    _body = "";
    if (pathIsFile(_path) || pathIsDirectory(_path))
    {
        if (remove(_path.c_str()))
        {
            if (!readErrorPage(_errorPage["403"]))
                _header = writeHeader("403 Forbidden", "text/html", _body.length());
        }
        _header = "HTTP/1.1 204 No Content\r\n\r\n";
    }
    else
    {
        if (!readErrorPage(_errorPage["404"]))
            _header = writeHeader("404 Not Found", "text/html", _body.length());
    }
    _response = _header + _body;
}

void    Response::putMethod()
{
    std::ofstream fd;
    std::stringstream buffer;

    _body = "";
    if (pathIsFile(_path))
    {
        fd.open(_path.c_str());
         if (fd.is_open() == false)
        {
            if (!readErrorPage(_errorPage["403"]))
                _header = writeHeader("403 Forbidden", "text/html", _body.length());
            _response = _header + _body;
            return ;
        }
        fd << _req.getBody();
        fd.close();
        _header = "HTTP/1.1 204 No Content\r\n\r\n";
    }
    else
    {
        fd.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (fd.is_open() == false)
        {
            if (!readErrorPage(_errorPage["403"]))
                _header = writeHeader("403 Forbidden", "text/html", _body.length());
            _response = _header + _body;
            return ;
        }
        fd << _req.getBody();
        fd.close();
        _header = "HTTP/1.1 201 Created\r\n\r\n";
    }
    _response = _header + _body;
}

int      Response::initRequest(Request &req)
{
     _req = req;
    _path = req.getFullPath();
	_generalRoot = req.getGeneralRoot();
    if (_req.getPath().empty() || _req.getMethod().empty() || _req.getProtocolVersion() != "HTTP/1.1")
    {
        if (!readErrorPage(_errorPage["400"]))
            _header = writeHeader("400 Bad Request", "text/html", _body.length());
        _response = _header + _body;
        return (1);
    }
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
    return (0);
}
void    Response::headMethod()
{
    readContent(_path);
    _response = _header;
}

void    Response::optionMethod()
{
    std::string Allow = "Allow: ";
    vec_str::iterator it = _allowMethods.begin();
    vec_str::iterator ite = _allowMethods.end();
    for (; it != ite; it++)
    {
        Allow += *it;
        if (it != ite - 1)
            Allow += ", ";
    }
    readContent(_path);
    if (_header.find("40") == std::string::npos)
        _header = _header.substr(0, _header.size() - 2) + Allow + "\r\n\r\n";
    _response = _header;
}

void     Response::makeAnswer(Request &req)
{
    if (initRequest(req))
        return ;
    if (_req.getBody().size() > _serv.getMaxBodySize())
    {
        if (!readErrorPage(_errorPage["413"]))
            _header = writeHeader("413 Payload Too Large", "text/html", _body.length());
        _response = _header + _body;
        return;
    }
    
    typedef void (Response::*ptr)();
    std::map<std::string, ptr> methods;
    methods["GET"] = &Response::getMethod;
    methods["POST"] = &Response::postMethod;
    methods["DELETE"] = &Response::deleteMethod;
    methods["PUT"] = &Response::putMethod;
    methods["HEAD"] = &Response::headMethod;
    methods["OPTIONS"] = &Response::optionMethod;

    if (methods.find(_req.getMethod()) != methods.end() && isAllow(_req.getMethod()))
        (this->*methods[_req.getMethod()])();
    else
    {
        if (!readErrorPage(_errorPage["405"]))
            _header = writeHeader("405 Method Not Allowed", "text/html", _body.length());
        _response = _header + _body;
    }
}

std::string&    Response::getResponse() { return _response;}

// Utils

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
