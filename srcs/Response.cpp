/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 18:33:19 by viroques          #+#    #+#             */
/*   Updated: 2022/02/08 21:20:35 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


/**************************************/
//           FUNCTIONS                //
/**************************************/

Response::Response(Server &serv): _serv(serv), _errorPage(serv.getErrorPage()) 
{
	_methods["GET"] = &Response::getMethod;
	_methods["POST"] = &Response::postMethod;
	_methods["DELETE"] = &Response::deleteMethod;
	_methods["HEAD"] = &Response::headMethod;
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

int      Response::initRequest(Request &req)
{
    _request = req;
    _path = req.getFullPath();
	_generalRoot = req.getGeneralRoot();
    if (_request.getPath().empty() || _request.getMethod().empty() ||
		_request.getProtocolVersion() != "HTTP/1.1" || _request.getHost().empty())
    {
		sendPage(400);
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
    if (_request.getBody().size() > _serv.getMaxBodySize())
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

void        Response::setLocationConf()
{
	std::vector<Location> loc = _serv.getLocation();
	std::vector<Location>::iterator it = loc.begin();
	std::string short_path = _request.getPath();
	size_t pos = short_path.length();

	while (pos != 0) {
		pos = short_path.rfind("/", pos - 1);
		for (; it != loc.end(); it++) {
			if (!strncmp(short_path.c_str(), it->getLocationMatch().c_str(), pos)) {
				break ;
			}
			if (it->getLocationMatch() == "/")
			{
				_pathCgi = it->getCgiHandler().second;
				_extensionCgi = it->getCgiHandler().first;
				_index = it->getIndex();
				_root = it->getRoot();
				_AutoIndex = it->getAutoIndex();
				_allowMethods = it->getMethods();
				_uploadDest = it->getUploadDest();
			}
		}
	}
	if (it != loc.end()) {
	_pathCgi = it->getCgiHandler().second;
	_extensionCgi = it->getCgiHandler().first;
	_index = it->getIndex();
	_root = it->getRoot();
	_AutoIndex = it->getAutoIndex();
	_allowMethods = it->getMethods();
	_uploadDest = it->getUploadDest();
	}
}

int      Response::readErrorPage(std::string &path)
{
	std::ifstream       ofs;
	std::stringstream   buffer;

	if (pathIsFile(path))
	{
		ofs.open(path.c_str(), std::ifstream::in);
		if (ofs.is_open() == false)
		{
			_body = "<!DOCTYPE html>\n<html><title>500</title><body>Something went wrong when finding error pages</body></html>\n";
			_header.setStatusError("500", _body.length());
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
		fd.open(path.c_str(), std::ifstream::in);
		if (fd.is_open() == false || pathIsDirectory(path))
		{
			sendPage(403);
			return ;
		}
		buffer << fd.rdbuf();
		fd.close();
		_body = buffer.str();
		_header.setHeader("200 OK", _request.getContentType(), _body.length());
	}
	else
	{
		sendPage(404);
	}
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
		_body = "<html><body>Your file has been upload!</body></html>";
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
		_body = "<html><body><h1>File deleted.</h1></body></html>";
		_header.setStatus("204 No Content");
	}
	else
	{
		sendPage(404);
	}
}

void    Response::headMethod()
{
	readContent(_path);
	_response = _header.getHeader();
}

void     Response::makeAnswer(Request &req)
{
    if (initRequest(req) == 0
		&& _methods.find(_request.getMethod()) != _methods.end() 
		&& isAllow(_request.getMethod()))
	{
		(this->*_methods[_request.getMethod()])();
	}
    else
		sendPage(405);
	_header.writeHeader();
	_response = _header.getHeader() + _body;
}

std::string&    Response::getResponse() { return _response;}


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
	std::cout << "LOCATION: " << _location << std::endl;
	_date = getTime();
    return (0);
}
