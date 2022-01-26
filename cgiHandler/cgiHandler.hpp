#ifndef CGI_HANDLER_H
# define CGI_HANDLER_H

# include "../request_parser/Request_Parser.hpp"
# include <unistd.h>
# include "../utils/utils.hpp"

// Open include
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// fileno inclde
# include <stdio.h>

class cgiHandler
{
private:

    std::map<std::string, std::string>  _env;
    std::string                         _body;

public:

    cgiHandler(Request &req);

    char            **envToString();
    char            **keyMapConvert(std::string key);
    std::string     execute(std::string pathToBinaryCgi);
};

#endif
