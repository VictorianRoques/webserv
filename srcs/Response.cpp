/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/23 20:18:28 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response() {}

Response::Response(int code)
{
	if (code == 400)
		sendPage(400);
	else
		sendPage(500);
	writeAnswer();
}

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

int		Response::initRequest(Request &req)
{
    _request = req;
    _path = req.getFullPath();
	_generalRoot = req.getGeneralRoot();
	_type = _path.substr(_path.rfind(".") + 1, _path.size() - _path.rfind("."));
    if (_request.getPath().empty() || _request.getMethod().empty()
		|| _request.getProtocolVersion() != "HTTP/1.1"
		|| (_request.getHost().empty() && _request.getMethod() != "DELETE"))
    {
		sendPage(400);
		return (1);
    }
    setLocationConf();
    if (static_cast <size_t>(_request.getBody().size()) > static_cast <size_t> (_serv.getMaxBodySize())
		|| static_cast <size_t>(_request.getBody().size()) > static_cast <size_t> (_loc.getMaxBodySize()))
    {
		sendPage(413);
        return (1);
    }
	if (_request.getRedirCode() == 310)
	{
		sendPage(310);
		return (1);
	}
    return (0);
}

int		Response::requestType(Request &req)
{
	if (initRequest(req))
	{
		writeAnswer();
		return (0);
	}
	if (needCgi())
	{
		return (1);
	}
	answer();
	return (0);
}

int		Response::needCgi()
{
	if (_extensionCgi.empty() == false &&
		_request.getFullPath().find(_extensionCgi) != std::string::npos)
	{
		return (1);
	}
	return (0);
}

void	Response::answer()
{
	if (_methods.find(_request.getMethod()) != _methods.end() 
		&& isAllow(_request.getMethod()))
	{
		(this->*_methods[_request.getMethod()])();
	}
    else
		sendPage(405);
	writeAnswer();
}

void	Response::sendPage(int code)
{
	std::string foundCode = std::to_string(code);
	if (!readErrorPage(_errorPage[foundCode]))
	{
		if (code == 308 || code == 310)
			_header.setStatusRedirect(foundCode, _body.length());
		else
			_header.setStatusError(foundCode, _body.length());
	}
}

int      Response::readErrorPage(std::string &path)
{
	std::ifstream       ifs;
	std::stringstream   buffer;

	if (pathIsFile(path))
	{
		ifs.open(path.c_str(), std::ios::binary);
		if (ifs.is_open() == false)
		{
			_body = "<!DOCTYPE html>\n<html><title>500</title><body><h1>Server Internal Error</h1></body></html>";
			_header.setStatusError("500", _body.length());
			return (1);
		}
		buffer << ifs.rdbuf();
		ifs.close();
		_body = buffer.str();
		ifs.close();
		return (0);
	}
	else
	{
		_body = "<!DOCTYPE html>\n<html><title>500</title><body><h1>Server Internal Error</h1></body></html>";
		_header.setStatusError("500", _body.length());
		return (1);
	}
}

void     Response::readContent(std::string &path)
{
	std::ifstream       fd;
	std::stringstream   buffer;

	if (pathIsFile(path) || pathIsDirectory(path))
	{
		fd.open(path.c_str(), std::ios::binary);
		if (fd.is_open() == false || pathIsDirectory(path))
		{
			sendPage(403);
			return ;
		}
		buffer << fd.rdbuf();
		fd.close();
		_body = buffer.str();
		_header.setHeader("200 OK", _request.getContentType(), _body.length());
		_header.searchContentType(_path);
	}
	else
	{
		sendPage(404);
	}
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
		readContent(_path);
	}
	if (_request.getRedirCode() == 308 && _type != "css")
		_header.setStatus("308 Permanent Redirect");
}

void		Response::deleteMethod()
{
	if (pathIsFile(_path) || pathIsDirectory(_path))
	{
		if (remove(_path.c_str()))
		{
			sendPage(403);
			return ;
		}
		_header.setStatus("204 No Content");
		_body = "";
	}
	else
	{
		sendPage(404);
	}
}

void		Response::postMethod()
{
	int ret;
	if ((ret = upload()) != 1)
    {
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
	_loc = loc;
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

void		Response::makeResponseCgi(std::string &answer)
{
	_header.setStatus("200 OK");
	isUpload(answer);
	_header.setCgiHeader(answer.substr(0, answer.find("\r\n\r\n")));
    _body = answer.substr(answer.find("\r\n\r\n") + 4);
	_header.setBodyLength(_body.size());
	writeAnswer();
}

std::string&		Response::getData() 			{ return _response; }

void				Response::writeAnswer()
{
	_header.writeHeader();
	_response = _header.getHeader() + _body;
}

void				Response::isUpload(std::string &answer)
{
	if (_request.getUploadDest().empty() == true 
		|| answer.find("File is valid, and was successfully uploaded.") == std::string::npos)
		return ;
	std::string contentType = _request.getContentType();
	size_t foundFileName = _request.getBody().find("filename=");
    if (foundFileName == std::string::npos || 
		contentType.find("multipart/form-data") == std::string::npos) 
        return ;
	std::string fileName = _request.getBody().substr(foundFileName, _request.getBody().find("\n", foundFileName));
    fileName = fileName.substr(0, fileName.find("\n"));
    fileName = fileName.substr(10);
    fileName.erase(fileName.length() - 2);
	if (fileName.empty())
		return ;
	_uploadDest = cleanSlash(_request.getUploadDest() + "/") + fileName;
	_location = "http://" + _request.getHost() + "/" + _uploadDest;
	_location = cleanSlash(_location);
	_header.setLocation(_location);
	_header.setDate();
	_header.setStatus("201 Created");
	return ;
}

int			Response::upload()
{
	if (_uploadDest.empty() == true)
		return (1);
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
    myfile.open(_uploadDest.c_str(), std::ios::binary);
    if (myfile.is_open() == false)
        return (2);
    myfile << body;
    myfile.close();
	_location = "http://" + _request.getHost() + "/" + _uploadDest;
	_location = cleanSlash(_location);
	_header.setDate();
	_header.setLocation(_location);
    return (0);
}
