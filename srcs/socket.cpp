/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:04:05 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/05 02:45:16 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

/*
** If an error occurs before the server is actually started,
** all the socket fds are closed before exit
*/
void	closeListen(std::vector<int> sockListen, size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen[i]);
	}
}

/*
** A socket can only listen on one port at a time
** so we bind a socket with every ports specified
*/
std::vector<int>	sockListen(std::vector<size_t>	ports)
{
	std::vector<int>	sockArr;
	int					sockTmp;
	sockaddr_in			addr;
	for (size_t i = 0; i < ports.size(); ++i) {
		if ((sockTmp = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
			perror("socket");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		int opt = 1;
		if (setsockopt(sockTmp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR) {
			perror("setsockopt");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(ports[i]);
		if (bind(sockTmp, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == ERROR) {
			perror("bind");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		if (listen(sockTmp, 1024) == ERROR) {
			perror("listen");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		sockArr.push_back(sockTmp);
	}
	return sockArr;
}

/*
** Sockets starting function
*/
void	sockServ(std::vector<Server> servers, std::vector<size_t> ports)
{
	SockData	sockData;
	sockData.setServers(servers);
	sockData.setSockListen(sockListen(ports));
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
