/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Meta.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 12:17:19 by user42            #+#    #+#             */
/*   Updated: 2022/01/18 14:53:08 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef META_HPP
# define META_HPP

# include "../utils/utils.hpp"
# include "Server.hpp"

class Server;

class Meta {
	//Coplian Class
	public:
		Meta();
		~Meta();
		Meta(const Meta &);
		Meta	&operator=(const Meta &);

	private:
		std::vector<Server *>	_servers;

		// getters
	public:
		std::vector<Server *>	getServers() const;

		//setters
	private:
		void setServers(std::vector<Server *> servers);

		// adding a server that has just been parsed
	public:
		void	addServer(Server server);

		// utils
	public:
		void	print_servers();
};

#endif
