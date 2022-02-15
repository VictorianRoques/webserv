/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/15 14:46:47 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response() {}

Response::Response(Server &serv): _serv(serv), _errorPage(serv.getErrorPage()), _code(0) {
	
	_methods["GET"] = &Response::getMethod;
// 	_methods["POST"] = &Response::postMethod;
// 	_methods["DELETE"] = &Response::deleteMethod;
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
bool			Response::getAutoIndex()	{ return _autoIndex; }
vec_str&		Response::getAllowMethods()	{ return _allowMethods; }
std::string&	Response::getUploadDest()	{ return _uploadDest; }
ResponseHeader&	Response::getHeader() 		{ return _header; }
std::string&	Response::getLocation()		{ return _location; }

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
		}
	}
	else
	{
		_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
		_header.setHeader("500 Internal Servor Error", "text/html", _body.length());
	}
}

int		Response::searchFd(Request &req)
{
	if (initRequest(req))
		return _fd;
	if(_methods.find(_request.getMethod()) != _methods.end() 
		&& isAllow(_request.getMethod()))
	{
		(this->*_methods[_request.getMethod()])();
	}
    else
		setFdError(405);
	return _fd;
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
    if (_request.getBody().size() > _serv.getMaxBodySize())
    {
		setFdError(413);
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

void		Response::getMethod()
{
	if (_autoIndex == true && pathIsDirectory(_path))
	{
		_body = autoIndexBuilder(_path);
		_header.setHeader("200 OK", "text/html", _body.length());
	}
	else
	{
		setFdContent();
	}
	if (_request.getRedirCode() == 308)
		_header.setStatus("308 Permanent Redirect");
}

void		Response::setFdContent()
{
	std::cout << "PATH: " << _path << std::endl;
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

void		Response::readFd()
{
	// Need to change to unsigned char
	char buffer[32000];
	size_t ret = 0;
	
	if (_fd)
	{
		ret = read(_fd, buffer, 3199);
		buffer[ret] = '\0';
		_body = buffer;
		_header.setBodyLength(_body.length());
	}
}

void		Response::makeAnswer()
{
	if (_body.empty() == true && _fd)
		readFd();
	_header.writeHeader();
	_response = _header.getHeader() + _body;
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
