#ifndef RESPONSE_HEADER_H
#define RESPONSE_HEADER_H

#include <iostream>

class ResponseHeader {

public:

    ResponseHeader() {};

private:

    std::string     allow;
    std::string     contentLanguage;
    std::string     contentLength;
    std::string     contentLocation;
    std::string     contentType;
    std::string     date;
    std::string     lastModified;
    std::string     location;
    std::string     retryAfter;
    std::string     server;
    std::string     transferEncoding;
    std::string     wwwAuthenticate;
    
};

#endif
