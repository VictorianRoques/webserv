/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:04:05 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/10 18:16:57 by fhamel           ###   ########.fr       */
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
		sockData.setRecvToActive();
		if (select(FD_SETSIZE,
		sockData.getRecvSet(), sockData.getSendSet(),
		NULL, NULL) != ERROR) {
			for (int fd = 0; fd < FD_SETSIZE; ++fd) {
				if (sockData.isRecvSet(fd)) {
					if (sockData.isSockListen(fd)) {
						sockData.addClient(fd);
					}
					else {
						sockData.recvClient(fd);
					}
				}
				else if (sockData.isSendSet(fd)) {
					sockData.sendClient(fd);
				}
			}
		}
	}
}
