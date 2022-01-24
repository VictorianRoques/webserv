#include "Response.hpp"

Response::Response(Request &req): _req(req) {

    _status = "OK";
    _code = 200;
    _path = req.getFullPath();
	std::cout << "CONSTRUCTOR:" << _path << std::endl;
    _contentType = req.getContentType();
}

std::string Response::readHtml(std::string &path)
{
    std::ifstream       ofs;
    std::stringstream   buffer;

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

    if (pathIsFile(path))
    {
        ofs.open(path.c_str(), std::ifstream::in);
        if (ofs.is_open() == false)
        {
            _code = 403;
            return "<!DOCTYPE html>\n<html><title>403 Forbiden</title><body><h1>Forbiden access</h1></body></html>\n";
        }
        buffer << ofs.rdbuf();
        ofs.close();
        _code = 200;
        return (buffer.str());
    }
    _code = 404;
    return "<!DOCTYPE html>\n<html><title>404 Not Found</title><body><h1>Ressource not found</h1></body></html>\n";
}

std::string     Response::writeHeader()
{
    std::string header;

    header = "HTTP/1.1 " + intToString(_code) + " " + _status + "\n";
    header += "Content-Type: " +    _contentType + "\n";
    header += "Content-Length: " + intToString(_bodyLength) + "\n";
    return header;
}

std::string     Response::getMethod()
{
    if (_req.getFullPath().find(".php") != std::string::npos)
    {
        cgiHandler cgi(_req);
        _response = cgi.execute("../cgi/darwin_phpcgi");
		std::cout <<"RESPOOOOOOOOOOOOOOOOOOOOOOOOOOONSE\n" <<  _response << std::endl;
        std::size_t pos = _response.find("\r\n");
       _response =  _response.substr(pos);
    }
     _response = readContent(_path);
    _bodyLength = _response.length();
    _response = writeHeader() +  "\n" + _response;
    return _response;
}

// Utils

std::string Response::intToString(int number)
{
    std::ostringstream convert;
    convert << number;
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
