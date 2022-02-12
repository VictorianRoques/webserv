/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorianroques <victorianroques@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/12 11:45:19 by victorianro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response() {}

Response::Response(Server &serv): _serv(serv), _errorPage(serv.getErrorPage()) {
	
	_methods["GET"] = &Response::getMethod;
	_methods["POST"] = &Response::postMethod;
	_methods["DELETE"] = &Response::deleteMethod;
}
Response::~Response() {}

Response::Response(const Response &res) { *this = res; }

Response	&Response::operator=(const Response &res)
{
	_request = res._request;
	_path = res._path;
	_body = res._body;
	_response = res._response;
	_serv = res._serv;
	_errorPage = res._errorPage;
	_pathCgi = res._pathCgi;
	_extensionCgi = res._extensionCgi;
	_index = res._index;
	_root = res._root;
	_generalRoot = res._generalRoot;
	_AutoIndex = res._AutoIndex;
	_allowMethods = res._allowMethods;
	_uploadDest = res._uploadDest;
	_header = res._header;
	_location = res._location;
	return *this;
}


Request&		Response::getRequest()		{ return _request; }
std::string&	Response::getPath()			{ return _path; }
std::string&	Response::getBody()			{ return _body; }
std::string&	Response::getResponse()		{ return _response; }
Server&			Response::getServ()			{ return _serv; }
map_str&		Response::getErrorPage()	{ return _errorPage; }
std::string&	Response::getPathCgi()		{ return _pathCgi; }
std::string&	Response::getExtensionCgi()	{ return _extensionCgi; }
std::string&	Response::getIndex()		{ return _index; }
std::string&	Response::getRoot()			{ return _root; }
std::string&	Response::getGeneralRoot()	{ return _generalRoot; }
bool			Response::getAutoIndex()	{ return _AutoIndex; }
vec_str&		Response::getAllowMethods()	{ return _allowMethods; }
std::string&	Response::getUploadDest()	{ return _uploadDest; }
ResponseHeader&	Response::getHeader() 		{ return _header; }
std::string&	Response::getLocation()		{ return _location; }

void	Response::defaultError()
{
	_code = 404;
	_fd = open(_errorPage["404"].c_str(), O_RDONLY);
	// FD_SET(_fd, _sendSet);
}

int		Response::initFd(Request &req, fd_set *sendSet)
{
	_sendSet = sendSet;
	if (initRequest(req))
	{
		defaultError();
		return (0);
	}
	else
	{
		readContent(_path);
	}
	return (0);
}

void	Response::readContent(std::string &path)
{
	if (pathIsFile(path))
	{
		_fd = open(path.c_str(), O_RDONLY);
		if (_fd < 0)
		{
			_code = 403;
			return ;
		}
		_code = 200;
		// FD_SET(_fd, _sendSet);
	}
	else
	{
		defaultError();
	}
}

void	Response::legalRead()
{
	char buff[4096];

	_contentLength = read(_fd, buff, 4096);
	buff[_contentLength] = '\0';
	_body = buff;
	if (_code >= 400)
	{
		_header.setStatusError(_code, _contentLength);
	}
	else
	{
		_header.setHeader("200 OK", _request.getContentType(), _contentLength);
	}
}

void     Response::makeAnswer()
{
	if (_fd >= 0 && _body.empty())
		legalRead();
	_header.writeHeader();
	_response = _header.getHeader() + _body;
}

int      Response::initRequest(Request &req)
{
    _request = req;
    _path = req.getFullPath();
	_generalRoot = req.getGeneralRoot();
    if (_request.getPath().empty() || _request.getMethod().empty()
		|| _request.getProtocolVersion() != "HTTP/1.1"
		|| (_request.getHost().empty() && _request.getMethod() != "DELETE"))
    {
		_code = 400;
		return (1);
    }
    setLocationConf();
    if (req.getPath() == "/")
    {
		if (_AutoIndex == true)
            _path.erase(_path.size() - 1);
        else
            _path = _root + "/" + _index;
    }
    if ((int)_request.getBody().size() > _serv.getMaxBodySize())
    {
		_code = 413;
        return (1);
    }
	if (_request.getRedirCode() == 310)
	{
		_body = "<html><body>Too many Redirects, infinite loop</body></html>";
		_header.setStatusRedirect("310", _body.length());
		_code = 310;
		return (1);
	}
    return (0);
}

void        Response::setLocationConf()
{
	Location loc = findRightLocation(_serv.getLocation(), _request);

	_pathCgi = loc.getCgiHandler().second;
	_extensionCgi = loc.getCgiHandler().first;
	_index = loc.getIndex();
	_root = loc.getRoot();
	_AutoIndex = loc.getAutoIndex();
	_allowMethods = loc.getMethods();
	_uploadDest = loc.getUploadDest();
}

void	Response::sendPage(int code)
{
	std::string foundCode = std::to_string(code);
	if (!readErrorPage(_errorPage[foundCode]))
	{
		if (code == 308 || code == 310)
			_header.setStatusRedirect(foundCode, _body.length());
		else
			_header.setStatusError(code, _body.length());
	}
}


int			Response::readErrorPage(std::string &path)
{
	if (pathIsFile(path))
	{
		_fd = open(path.c_str(), O_RDONLY);
		if (_fd < 0)
		{
			_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
			_header.setStatusError(500, _body.length());
			return (1);
		}
	}
	else
	{
		_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
		_header.setStatusError(500, _body.length());
		return (1);
	}
	FD_SET(_fd, _sendSet);
	return (0);
}


void        Response::getMethod()
{
    if (_extensionCgi.empty() == false && _request.getFullPath().find(_extensionCgi) != std::string::npos) {
		cgiHandler cgi(_request);
        _body = cgi.execute(_pathCgi);
        if (_body == "<html><body>FATAL ERROR CGI</body></html>")
        {
			sendPage(500);
            return ;
        }
		_header.setCgiHeader(_body.substr(0, _body.find("\r\n\r\n")));
        _body = _body.substr(_body.find("\r\n\r\n") + 4);
		_header.setBodyLength(_body.length());
    }
    else if (_AutoIndex == true && pathIsDirectory(_path))
    {
        _body = autoIndexBuilder(_path);
		_header.setHeader("200 OK", "text/html", _body.length());
    }
    else {
		readContent(_path);
    }
	if (_request.getRedirCode() == 308)
		_header.setStatus("308 Permanent Redirect");
}

void     Response::postMethod()
{
    cgiHandler cgi(_request);
    if (_extensionCgi.empty() == false && _path.find(_extensionCgi) != std::string::npos) 
    {
        _body = cgi.execute(_pathCgi);
        if (_body == "<html><body>FATAL ERROR CGI</body></html>")
        {
			sendPage(500);
            return ;
        }
		_header.setCgiHeader(_body.substr(0, _body.find("\r\n\r\n")));
        _body = _body.substr(_body.find("\r\n\r\n") + 4);
		_header.setBodyLength(_body.length());
    }
    else if (_uploadDest.empty() == false) 
    {
		int ret = upload();
		if (ret == 1)
		{
			sendPage(400);
			return ;
		}
		if (ret == 2)
		{
			sendPage(403);
			return ;
		}
		_body = hrefLocation(_uploadDest);
		_header.setHeader("201 Created", "text/html", _body.length());
    }
	else
	{
		_header.setStatus("204 No Content");
	 	_body = "";
	}
}

void        Response::deleteMethod()
{
	_body = "";
	if (pathIsFile(_path) || pathIsDirectory(_path))
	{
		if (remove(_path.c_str()))
		{
			sendPage(403);
			return ;
		}
		_header.setStatus("204 No Content");
	}
	else
	{
		sendPage(404);
	}
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

int			Response::upload()
{
	std::string contentType = _request.getContentType();
	std::string body = _request.getBody();
	size_t foundFileName = body.find("filename=");
    if (foundFileName == std::string::npos || 
		contentType.find("multipart/form-data") == std::string::npos) 
        return (1);
    
    std::string fileName = body.substr(foundFileName, body.find("\n", foundFileName));
    fileName = fileName.substr(0, fileName.find("\n"));
    fileName = fileName.substr(10);
    fileName.erase(fileName.length() - 2);
	if (fileName.empty())
		return (1);
	
    _uploadDest = _uploadDest + "/" + fileName;
    std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
    body = body.substr(body.find("\r\n\r\n") + 4);
    body = body.substr(0, body.find(boundary));
    body = body.substr(0, body.length() - 2);
		
    std::ofstream myfile;
    myfile.open(_uploadDest.c_str());
    if (myfile.is_open() == false)
        return (2);
    myfile << body.c_str();
    myfile.close();
	_location = "http://" + _request.getHost() + "/" + _uploadDest;
	_location = cleanSlash(_location);
	_header.setDate();
    return (0);
}
