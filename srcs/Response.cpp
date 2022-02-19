/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/19 22:06:27 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response() {}

Response::Response(Server &serv): _serv(serv), _errorPage(serv.getErrorPage()), _code(0) {
	
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
	_autoIndex = res._autoIndex;
	_allowMethods = res._allowMethods;
	_uploadDest = res._uploadDest;
	_header = res._header;
	_location = res._location;
	return *this;
}


Request&		Response::getRequest()			{ return _request; }
std::string&	Response::getPath()				{ return _path; }
std::string&	Response::getBody()				{ return _body; }
std::string&	Response::getResponse()			{ return _response; }
Server&			Response::getServ()				{ return _serv; }
map_str&		Response::getErrorPage()		{ return _errorPage; }
std::string&	Response::getPathCgi()			{ return _pathCgi; }
std::string&	Response::getExtensionCgi()		{ return _extensionCgi; }
std::string&	Response::getIndex()			{ return _index; }
std::string&	Response::getRoot()				{ return _root; }
std::string&	Response::getGeneralRoot()		{ return _generalRoot; }
bool			Response::getAutoIndex()		{ return _autoIndex; }
vec_str&		Response::getAllowMethods()		{ return _allowMethods; }
std::string&	Response::getUploadDest()		{ return _uploadDest; }
ResponseHeader&	Response::getResponseHeader()	{ return _header; }
std::string&	Response::getLocation()			{ return _location; }

int		Response::searchFd(Request &req)
{
	if (initRequest(req))
		return _fd;
	if (needCgi())
	{
		return (-1);
	}
	answer();
	return _fd;
}

void	Response::answer()
{
	if (_methods.find(_request.getMethod()) != _methods.end() 
		&& isAllow(_request.getMethod()))
	{
		(this->*_methods[_request.getMethod()])();
		if (_fd == -2)
		{
			_header.writeHeader();
			_response = _header.getHeader() + _body;
		}
	}
    else
		setFdError(405);
}

int		Response::needCgi()
{
	if (_extensionCgi.empty() == false &&
		_path.find(_extensionCgi) != std::string::npos)
	{
		return (1);
	}
	return (0);
}

int		Response::initRequest(Request &req)
{
    _request = req;
    _path = req.getFullPath();
	_generalRoot = req.getGeneralRoot();
    if (_request.getPath().empty() || _request.getMethod().empty()
		|| _request.getProtocolVersion() != "HTTP/1.1"
		|| (_request.getHost().empty() && _request.getMethod() != "DELETE"))
    {
		setFdError(400);
		return (1);
    }
    setLocationConf();
    if (req.getPath() == "/")
    {
		if (_autoIndex == true)
            _path.erase(_path.size() - 1);
        else
            _path = _root + "/" + _index;
    }
    if ((size_t)_request.getBody().size() > (size_t)_serv.getMaxBodySize())
    {
		setFdError(413);
        return (1);
    }
	if (_request.getRedirCode() == 310)
	{
		_body = "<html><body>Too many Redirects, infinite loop</body></html>";
		_header.setStatusRedirect("310", _body.length());
		_code = 310;
		_fd = -2;
		return (1);
	}
    return (0);
}

void	Response::setFdError(int code)
{
	_code = code;
	std::string path = _errorPage[std::to_string(_code)];
	if (pathIsFile(path))
	{
		_fd = open(path.c_str(), O_RDONLY);
		_header.setStatusError(std::to_string(code));
		if (_fd < 0)
		{
			_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
			_header.setHeader("500 Internal Servor Error", "text/html", _body.length());
			_fd = -2;
		}
	}
	else
	{
		_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
		_header.setHeader("500 Internal Servor Error", "text/html", _body.length());
		_fd = -2;
	}
}

void		Response::setFdContent()
{
	if (pathIsFile(_path))
	{
		_fd = open(_path.c_str(), O_RDONLY);
		if (_fd < 0)
		{
			setFdError(403);
			return ;
		}
		_header.setStatus("200 OK");
		_header.setContentType(_request.getContentType());
	}
	else
	{
		setFdError(404);
	}
}

void		Response::getMethod()
{
	if (_autoIndex == true && pathIsDirectory(_path))
	{
		_body = autoIndexBuilder(_path);
		_header.setHeader("200 OK", "text/html", _body.length());
		_fd = -2;
	}
	else
	{
		setFdContent();
	}
	if (_request.getRedirCode() == 308)
		_header.setStatus("308 Permanent Redirect");
}

void		Response::deleteMethod()
{
	if (pathIsFile(_path) || pathIsDirectory(_path))
	{
		if (remove(_path.c_str()))
		{
			setFdError(403);
			return ;
		}
		_header.setStatus("204 No Content");
		_body = "";
		_fd = -2;
	}
	else
	{
		setFdError(404);
	}
}

void		Response::postMethod()
{
 	if (_uploadDest.empty() == false) 
    {
		int ret = upload();
		if (ret == 1)
		{
			setFdError(400);
			return ;
		}
		if (ret == 2)
		{
			setFdError(403);
			return ;
		}
		_body = hrefLocation(_uploadDest);
		_header.setHeader("201 Created", "text/html", _body.length());
    }
	else
	{
		_header.setStatus("204 No Content");
		_body = "";
		_fd = -2;
	}
}

void        Response::setLocationConf()
{
	Location loc = findRightLocation(_serv.getLocation(), _request);

	_pathCgi = loc.getCgiHandler().second;
	_extensionCgi = loc.getCgiHandler().first;
	_index = loc.getIndex();
	_root = loc.getRoot();
	_autoIndex = loc.getAutoIndex();
	_allowMethods = loc.getMethods();
	_uploadDest = loc.getUploadDest();
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

void		Response::makeResponse(std::string &answer, bool cgi)
{
	if (cgi)
	{
		_header.setCgiHeader(answer.substr(0, answer.find("\r\n\r\n")));
    	_body = answer.substr(answer.find("\r\n\r\n") + 4);
		_header.setBodyLength(_body.length());
		_header.writeHeader();
		_response = _header.getHeader() + _body;
	}
	else
	{
		_header.setBodyLength(answer.length());
		_header.writeHeader();
		_response = _header.getHeader() + answer;
	}
}

std::string&	Response::getData() 			{ return _response; }

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
	
	/*
		Open Create file, 201 Created
		open failed -> 403 
		Return fd , Florian write body dans ce fd !
		Return body "Your files have been Upload "
		+ setHeader()
	*/

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
