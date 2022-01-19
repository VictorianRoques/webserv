/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 17:59:04 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "../utils/utils.hpp"
# include "Server.hpp"

class Server;

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
	
		// getters
	public:
		bool						getInLocation() const;
		bool						getInServer() const;

		// PARSING
	public:
		// parsing main functions
		void tokenizer(char **av);
		void interpreter(vec_str tok);
		
		// parsing helper functions
		void	addPort(std::string port);
		void	clear();
		size_t	dirListen(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServerName(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirErrorPage(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirLocation(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServer(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirClose(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirOpen(vec_str::iterator it, vec_str::iterator vend);

		//EXCEPTION

		class MissingBracketException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
		
		class OutsideServerException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class LonelyBracketException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class EmbeddedServersException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
		
		class NoSuchDirectiveException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class FailedToOpenException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

};

# endif
