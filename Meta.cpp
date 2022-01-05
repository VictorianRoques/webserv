/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Meta.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 12:25:32 by user42            #+#    #+#             */
/*   Updated: 2022/01/05 21:12:25 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Meta.hpp"
#include "utils.hpp"

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

std::vector<Server>	Meta::getServers() const { return _servers; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Meta::setServers(std::vector<Server> servers) { _servers = servers; }

/**************************************/
//			SERVER PARSED			  //
/**************************************/

void	Meta::addServer(Server server) {
	_servers.push_back(server);
}

/**************************************/
//				UTILS				  //
/**************************************/

/**
 * print_servers(): prints every Server variables' value from vector '_server'
**/
void	Meta::print_servers() {
	std::vector<Server>::iterator	it;

	for (it = _servers.begin(); it != _servers.end(); it++) {
		it->print_serv();
	}
}
