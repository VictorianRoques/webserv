/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Meta.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 12:25:32 by user42            #+#    #+#             */
/*   Updated: 2022/01/18 17:51:21 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Meta.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Meta::Meta() {}

Meta::~Meta() {}

Meta::Meta(const Meta &x) {
	*this = x;
}

Meta&	Meta::operator=(const Meta &x) {
	if (this != &x)
		_servers = x.getServers();
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::vector<Server *>	Meta::getServers() const { return _servers; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Meta::setServers(std::vector<Server *> servers) { _servers = servers; }

/**************************************/
//			SERVER PARSED			  //
/**************************************/

void	Meta::addServer(Server server) {
	//DELETE FOLLOWING LINE WHEN DONE (used for testing)
	server.print_serv();
	Server *tmp = new Server(server);
	_servers.push_back(tmp);
	std::cout << _servers.size() << std::endl;
}

/**************************************/
//				UTILS				  //
/**************************************/

/**
 * print_servers(): prints every Server variables' value from vector '_server'
**/
void	Meta::print_servers() {
	std::vector<Server *>::iterator	it;

	std::cout << "PRINT SERVERS" << std::endl;
	std::cout << _servers.size() << std::endl;
	for (it = _servers.begin(); it != _servers.end(); it++) {
		(*it)->print_serv();
	}
}
