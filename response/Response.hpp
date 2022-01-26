#ifndef RESPONSE_H
#define RESPONSE_H

# include <iostream>
# include <fstream>
# include <sstream>

# include <map>


// PathIsfile Include
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>

# include "../request_parser/Request_Parser.hpp"
# include "../cgiHandler/cgiHandler.hpp"

class Response {

public:

    Response(Request &req, map_str errorPage);

    std::string     call();
    std::string     readHtml(std::string &path);
    std::string     readContent(std::string &path);
    std::string     writeHeader(std::string status, std::string contentType, size_t bodyLength);
    std::string     getMethod();
    std::string     postMethod();
    void            setCgiHeader(std::string cgiHeader);

    // Utils
    int             pathIsFile(std::string &path);
    std::string     sizeToString(size_t size);

private:

    std::string                 _bodyLength;
    Request                     _req;
    std::string                 _status;
    std::string                 _contentType;
    std::string                 _path;
    std::string                 _response;
    map_str                     _errorPage;

    // std::map<int, std::string>  _errorMap;
};

#endif
