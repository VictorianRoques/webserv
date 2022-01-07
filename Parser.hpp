/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/07 18:38:22 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "utils.hpp"
# include "Server.hpp"
# include "Meta.hpp"

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

		// Coplien Class
	public:
		Parser();
		~Parser();
		Parser(const Parser &);
		Parser&	operator=(const Parser &);

		// variables
	private:
		size_t						_serverNb;
	
		// getters
	public:
		size_t						getServerNb() const;

		// parsing
	public:
		// parsing main functions
		void tokenizer(char **av);
		void interpreter(vec_str tok);
		
		// parsing helper functions
		void	addPort(std::string port);
		size_t	dirListen(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServerName(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirErrorPage(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirLocation(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirServer(vec_str::iterator it, vec_str::iterator vend);
};

# endif
