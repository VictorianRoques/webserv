# webserv

42 Paris group project | C++98 implementation of a HTTP server

Notes about the server:

LISTEN
If no "listen port" directive is specified in the server {} in the config file, default port = 8080.

METHOD
If no "methods" directive is specified in the location {} in the config file, default method is GET.
If "methods" directive is specified, the specified methods will be the only methods allowed (if GET is not part of them, it will not be allowed anymore)

CLIENT MAX BODY SIZE
Can be defined for the server {} and for each location {}.
Default value: 1M (nginx default value)

AUTOINDEX
Default: on
