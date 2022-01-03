/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/03 17:43:58 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <cstdlib> // std::atoi()
# include <utility> // std::make_pair
# include <string>
# include <fstream>
# include <vector>
# include "Parser.hpp"

/**
 * type simplification (makes the code 'lighter')
**/
typedef std::vector<std::string>	vec_str;

/**
 * ft_putvec(): prints out the vector v
**/
template < class T >
void	ft_putvec(std::vector<T> v) {
	typename std::vector<T>::iterator it = v.begin();
		
	for (; it != v.end(); it++)
		std::cout << "<" << *it << ">" << std::endl;
}

/**
 * struct location: holds location directives
**/
typedef struct s_location {
	std::string root;
} t_location;


/**
 * class Parser: holds the server directives
**/
class Parser {

		// method pointer typedef
	public:
		typedef void (*methodPointer)(vec_str::iterator it, vec_str::iterator vend);

		// Coplien Class
	public:
		Parser();
		~Parser();
		Parser(const Parser &);
		Parser&	operator=(const Parser &);

		// characteristics
	private:
		size_t				_serverNb;
		std::string			_ip;
		std::vector<size_t>	_port;
		std::vector<string> _serverName;
		std::string			_root;
		std::vector<string>	_errorPage;
		size_t				_maxBodySize;
		t_location			_location;

		// getters
	public:
		std::string			getServerNb() const;
		std::string			getIP() const;
		std::vector<size_t>	getPort() const;
		std::vector<string>	getServerName() const;
		std::string			getRoot() const;
		std::vector<string>	getErrorPage() const;
		size_t				getMaxBodySize() const;
		t_location			getLocation() const;

		//setters
	private:
		void	setIP(std::string ip);
		void	setPort(std::string port);
		void	setServerName(vec_str serverName);
		void	setRoot(std::string root);
		void	setErrorPage(vec_str errorPage);
		void	setMaxBodySize(size_t maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		void	setLocation(t_location location);

		void	addPort(std::string port);
		size_t	dirListen(std::string listen);

		// parsing functions
	public:
		void tokenize(char **av);
		void interpreter(vec_str tok);
};

# endif
