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

    Response(Request &req);

    std::string     readHtml(std::string &path);
    std::string     readContent(std::string &path);
    std::string     writeHeader();
    std::string     getMethod();

    // Utils
    int             pathIsFile(std::string &path);
    std::string     intToString(int number);

private:

    int                         _bodyLength;
    int                         _code;

    Request                     _req;
    std::string                 _status;
    std::string                 _contentType;
    std::string                 _path;
    std::string                 _response;


    // std::map<int, std::string>  _errorMap;
};

#endif
