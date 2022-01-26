#include "Response.hpp"

Response::Response(Request &req, Server &serv): _req(req), _serv(serv), _errorPage(serv.getErrorPage())
{
    _status = "200 OK";
    _path = req.getFullPath();
    _contentType = req.getContentType();

    // FOUND CGI PATH
    std::vector<Location *> loc = serv.getLocation();
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
            }
		}
	}
    std::cout << "PATH CGI: " << _pathCgi << std::endl;
}

std::string Response::readHtml(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;
    std::cout << "PATH ERROR PAGES: " << path << std::endl;
    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
            return ("<!DOCTYPE html>\n<html><title>Error</title><body><h1>Something went wrong when reading error page</h1></body></html>\n"); 
        
        buffer << ofs.rdbuf();
        ofs.close();
        return (buffer.str());
    }
    else
        return ("<!DOCTYPE html>\n<html><title>Error</title><body><h1>Something went wrong when reading error page</h1></body></html>\n");
}

std::string     Response::readContent(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;
    std::string         error_res;

    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
        {
            error_res = "<!DOCTYPE html>\n<html><title>403 Forbiden</title><body><h1>Forbiden access</h1></body></html>\n";
             return  writeHeader("404 Not Found", "text/html", error_res.length()) +  "\r\n\r\n" + error_res;
        }
        buffer << ofs.rdbuf();
        ofs.close();
        return (buffer.str());
    }
    error_res = "<!DOCTYPE html>\n<html><title>404 Not Found</title><body><h1>Ressource not found</h1></body></html>\n";
    return  writeHeader("404 Not Found", "text/html", error_res.length()) +  "\r\n\r\n" + _response;

}

std::string     Response::writeHeader(std::string status, std::string contentType, size_t bodyLength)
{
    std::string header;

    header = "HTTP/1.1 " + status + "\r\n";
    header += "Content-Type: " + contentType + "\r\n";
    header += "Content-Length: " + sizeToString(bodyLength) + "\r\n";
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
std::string     Response::call()
{
    if (_req.getMethod() == "GET")
        return getMethod();
    else if (_req.getMethod() == "POST")
        return postMethod();
    else
    {
        std::string res = readHtml(_errorPage["405"]);
        return writeHeader("405 Method Not Allow", "text/html", res.length()) + "\r\n\r\n" + res;
    }
}

std::string     Response::postMethod()
{
    cgiHandler cgi(_req);
    _response = cgi.execute(_pathCgi);
     setCgiHeader(_response.substr(0, _response.find("\r\n\r\n")));      
    _response = _response.substr(_response.find("\r\n\r\n") + 4);
    return writeHeader(_status, _contentType, _response.length()) + "\r\n" + _response;
}

std::string     Response::getMethod()
{
    if (_req.getFullPath().find(_extensionCgi) != std::string::npos)
    {
        cgiHandler cgi(_req);
        _response = cgi.execute(_pathCgi);
        setCgiHeader(_response.substr(0, _response.find("\r\n\r\n")));      
        _response = _response.substr(_response.find("\r\n\r\n") + 4);
        return writeHeader(_status, _contentType, _response.length()) + "\r\n" + _response;
    }
    else
    {
        _response = readContent(_path);
        _response = writeHeader("200 OK", _contentType, _response.length()) +  "\r\n" + _response;
    }
    std::cout << "RESPONSE BEGIN\n" << _response << "\nRESPONSE END\n";
    return _response;
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
