/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/22 16:41:18 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

# include "utils.hpp"
# include "Server.hpp"
# include "Response.hpp"

class Server;

Request	requestParser(std::string request_header, std::vector<Server> _servers_g);

/**
 * class Parser: holds the server directives while parsing
**/
class Parser: public Server {

		/**
		 *  method pointer typedef: redirects to parsing helper functions (see below)
		 *  return type: size_t (number of elements iterated over in the vector... 
		 *  ...while calling the helper function)
		 *  params:
		 *  -it = current position in the vector
		 *  -vend = vector.end()
		**/
	public:
		typedef size_t	(Parser::*methodPointer)(vec_str::iterator it, vec_str::iterator vend);

		// Coplian Class
	public:
		Parser();
		~Parser();
		Parser(const Parser &);
		Parser&	operator=(const Parser &);

		// variables
	private:
		bool						_in_server;
		bool						_in_location;
		std::vector<Server>			_servers_g;
		std::vector<size_t>			_ports_g;
	
		// getters
	public:
		bool						getInLocation() const;
		bool						getInServer() const;
		std::vector<Server>			&getServersG();
		std::vector<size_t>			getPortsG();
		

		// PARSING
	public:
		// parsing main functions
		void tokenizer(char **av);
		void interpreter(vec_str tok);
		
		// parsing helper functions
		void	addPort(std::string port);
		void	clear();
		void	buildErrorPageContent();
		size_t	dirListen(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServerName(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirErrorPage(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirLocation(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServer(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirClose(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirOpen(vec_str::iterator it, vec_str::iterator vend);

		void	print();
	
		// CHECKERS
		void	serverChecker();
		bool	serverNameChecker();
		bool	locationChecker();
		bool	errorPageChecker();

		//EXCEPTION

		class MissingBracketException: public std::exception { public: virtual char const *what() const throw(); };
		
		class OutsideServerException: public std::exception { public: virtual char const *what() const throw(); };

		class LonelyBracketException: public std::exception { public: virtual char const *what() const throw(); };

		class EmbeddedServersException: public std::exception { public: virtual char const *what() const throw(); };
		
		class NoSuchDirectiveException: public std::exception { public: virtual char const *what() const throw(); };

		class FailedToOpenException: public std::exception { public: virtual char const *what() const throw(); };
		
		class FailedToCloseException: public std::exception { public: virtual char const *what() const throw(); };
		
		class WrongPathException: public std::exception { public: virtual char const *what() const throw(); };

		class ErrorPageException: public std::exception { public: virtual char const *what() const throw(); };

		class UnaccessiblePortException: public std::exception { public: virtual char const *what() const throw(); };

		class NeedOnePortException: public std::exception { public: virtual char const *what() const throw(); };
		
		class ConfigFileIsDirectoryException: public std::exception { public: virtual char const *what() const throw(); };

		class WrongServerNameException: public std::exception { public: virtual char const *what() const throw(); };

		class EmptyConfigException: public std::exception { public: virtual char const *what() const throw(); };

		class MissingDefaultLocationException: public std::exception { public: virtual char const *what() const throw(); };
		
		class ErrorPagePathException: public std::exception { public: virtual char const *what() const throw(); };

};

# endif
