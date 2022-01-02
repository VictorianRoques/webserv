/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/02 17:47:08 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include "Parser.hpp"

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
 * struct location: holds
**/
typedef struct s_location {
	std::string root;
} t_location;


/**
 * class Parser: holds the server characteristics
**/
class Parser {
	/** Coplien Class **/
	public:
		Parser();
		~Parser();
		Parser(const Parser &);
		Parser&	operator=(const Parser &);

		// characteristics
	private:
		std::string			_ip;
		size_t				_port;
		std::vector<string> _serverName;
		std::string			_root;
		std::vector<string>	_errorPage;
		size_t				_maxBodySize;
		t_location			_location;

		// getters
	public:
		std::string			getIP();
		size_t				getPort();
		std::vector<string>	getServerName();
		std::string			getRoot();
		std::vector<string>	getErrorPage();
		size_t				getMaxBodySize();
		t_location			getLocation();

		//setters
	private:
		void	setIP(std::string ip);
		void	setPort(size_t port);
		void	setServerName(std::vector<std::string> serverName);
		void	setRoot(std::string root);
		void	setErrorPage(std::vector<std::string> errorPage);
		void	setMaxBodySize(size_t maxBodySize);
		void	setLocation(t_location location);

		// parsing functions
	public:
		void tokenize(char **av);
		void interpreter(std::vector<std::string> tok);
};

# endif
