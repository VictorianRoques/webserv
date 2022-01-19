/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:15:20 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../utils/utils.hpp"
# include "Location.hpp" 

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

		// variables
	protected:
		std::string						_ip;
		std::vector<size_t>				_port;
		std::vector<std::string>	 	_serverName;
		std::string						_root;
		std::vector<std::string>		_errorPage;
		size_t							_maxBodySize;
		std::vector<Location *>			_location;
		int								_serverNb;

		// getters
	public:
		std::string						getIP() const;
		std::vector<size_t>				getPort() const;
		std::vector<std::string>		getServerName() const;
		std::vector<std::string>		getErrorPage() const;
		size_t							getMaxBodySize() const;
		std::vector<Location *>			getLocation() const;
		size_t							getServerNb() const;

		//setters
	protected:
		void	setIP(std::string ip);
		void	setPort(std::vector<size_t> port);
		void	setServerName(vec_str serverName);
		void	setErrorPage(vec_str errorPage);
		void	setMaxBodySize(size_t maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		void	setLocation(std::vector<Location *> location);
		void	setServerNb(size_t serverNb);

		// adding parsed server
	public:
//		void	addServer(Meta meta);

		// utils
	public:
		void	print_serv();
};



# endif
