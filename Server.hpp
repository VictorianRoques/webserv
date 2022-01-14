/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:16:26 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/14 12:06:20 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Location.hpp" //--> alternative: 'Location' is a nested class of 'Server'
# include "Meta.hpp"

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
		std::string						_ip;
		std::vector<size_t>				_port;
		std::vector<std::string>	 	_serverName;
		std::string						_root;
		std::vector<std::string>		_errorPage;
		size_t							_maxBodySize;
		bool							_autoIndex;
		std::vector<Location *>			_location;
		size_t							_serverNb;
		std::pair<size_t, std::string>	_redirection;

		// getters
	public:
		std::string						getIP() const;
		std::vector<size_t>				getPort() const;
		std::vector<std::string>		getServerName() const;
		std::string						getRoot() const;
		std::vector<std::string>		getErrorPage() const;
		size_t							getMaxBodySize() const;
		bool							getAutoIndex() const;
		std::vector<Location *>			getLocation() const;
		size_t							getServerNb() const;
		std::pair<size_t, std::string>	getRedirection() const;

		//setters
	protected:
		void	setIP(std::string ip);
		void	setPort(std::vector<size_t> port);
		void	setServerName(vec_str serverName);
		void	setRoot(std::string root);
		void	setErrorPage(vec_str errorPage);
		void	setMaxBodySize(size_t maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		void	setAutoIndex(bool autoIndex);
		void	setLocation(std::vector<Location *> location);
		void	setServerNb(size_t serverNb);
		void	setRedirection(std::pair<size_t, std::string> redirection);

		// adding parsed server
	public:
		void	addServer(Meta meta);

		// utils
	public:
		void	print_serv();
};

# endif
