/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/05 19:35:03 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdlib> // std::atoi()
# include <utility> // std::make_pair
# include <string>
# include <fstream>
# include <vector>
# include "Meta.hpp"
# include "Location.hpp" //--> alternative: 'Location' is a nested class of 'Server'

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

class Meta;
class Location;

/**
 * class Server: holds the server directives
**/
class Server {

		// Coplien Class
	public:
		Server();
		~Server();
		Server(const Server &);
		Server&	operator=(const Server &);

	/**
	 * NESTED CLASS: Location
	class Location {
		// Coplian Class
		public:
			Location();
			~Location();
			Location(const Location &);
			Location&	operator=(const Location &);
	
		// directives
		private:
			std::string		_root;
	
			// getters
		public:
			std::string		getRoot() const;
	
			//setters
		private:
			void			setRoot(std::string root);
	};
	//END OF NESTED CLASS
	**/

		// variables
	protected:
		size_t						_serverNb;
		std::string					_ip;
		std::vector<size_t>			_port;
		std::vector<std::string>	 _serverName;
		std::string					_root;
		std::vector<std::string>	_errorPage;
		size_t						_maxBodySize;
		Location*					_location;

		// getters
	public:
		size_t						getServerNb() const;
		std::string					getIP() const;
		std::vector<size_t>			getPort() const;
		std::vector<std::string>	getServerName() const;
		std::string					getRoot() const;
		std::vector<std::string>	getErrorPage() const;
		size_t						getMaxBodySize() const;
		Location*					getLocation() const;

		//setters
	protected:
		void	setIP(std::string ip);
		void	setPort(std::vector<size_t> port);
		void	setServerName(vec_str serverName);
		void	setRoot(std::string root);
		void	setErrorPage(vec_str errorPage);
		void	setMaxBodySize(size_t maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		void	setLocation(Location* location);

		// adding parsed server
	public:
		void	addServer(Meta meta);
};

# endif
