/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/10 19:36:08 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
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
		vec_str	 						_serverName;
		std::string						_root;
		map_str							_errorPage;
		long long						_maxBodySize;
		std::vector<Location>			_location;
		int								_serverNb;
		std::vector<Server>				_servers_g;
		map_str							_errorPageContent;

		std::string							_generalRoot;

		// getters
	public:
		std::string						getIP() const;
		std::vector<size_t>				getPort() const;
		vec_str							getServerName() const;
		map_str							getErrorPage() const;
		map_str							getErrorPageContent() const;
		long long						getMaxBodySize() const;
		std::vector<Location>			getLocation() const;
		size_t							getServerNb() const;

		std::string							getGeneralRoot() const;
		
		//setters
	protected:
		void	setIP(std::string ip);
		void	setPort(std::vector<size_t> port);
		void	setServerName(vec_str serverName);
		void	setErrorPage(map_str errorPage);
		void	setErrorPageContent(map_str errorPageContent);
		void	setMaxBodySize(long long maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		void	setLocation(std::vector<Location> location);
		void	setServerNb(size_t serverNb);

		void	setGeneralRoot(std::string generalRoot);

		// utils
	public:
		void	print_serv();

	//EXCEPTION

		class ExceededMaxBodySizeException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
		
		class WrongFormatMaxBodySizeException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

};



# endif
