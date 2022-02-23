/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:04:05 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/23 12:41:40 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

/*
** Sockets starting function
*/
void	sockServ(std::vector<Server> servers, std::vector<size_t> ports)
{
	SockData	sockData;
	sockData.setServers(servers);
	sockData.setSockListen(ports);
	for (size_t i = 0; i < sockData.getSizeListen(); ++i) {
		sockData.addActiveSet(sockData.getSockListen(i));
	}
	while (1) {
		sockData.setReadToActive();
		if (select(FD_SETSIZE,
		sockData.getReadSet(), sockData.getWriteSet(),
		NULL, NULL) != ERROR) {
			for (int fd = 0; fd < FD_SETSIZE; ++fd) {
				if (sockData.isReadSet(fd)) {
					if (sockData.isSockListen(fd)) {
						sockData.addClient(fd);
					}
					else {
						sockData.recvClient(fd);
					}
				}
				else if (sockData.isWriteSet(fd)) {
					sockData.sendClient(fd);
				}
			}
		}
		else {
			std::cerr << RED << "Server: select: error server down" << NC << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}
