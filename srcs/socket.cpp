/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:04:05 by fhamel            #+#    #+#             */
/*   Updated: 2022/01/27 12:35:43 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socket.hpp"
/*
** A socket can only listen on one port at a time
** so we bind a socket with every ports specified
*/
std::vector<int>	sockListen(std::vector<size_t>	port)
{
	std::vector<int>	sockArr;
	int					sockTmp;
	sockaddr_in			addr;
	for (size_t i = 0; i < port.size(); ++i) {
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
		addr.sin_port = htons(port[i]);
		if (bind(sockTmp, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == ERROR) {
			perror("bind");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		if (listen(sockTmp, 8) == ERROR) {
			perror("listen");
			closeListen(sockArr, i);
			exit(EXIT_FAILURE);
		}
		sockArr.push_back(sockTmp);
	}
	return sockArr;
}

/*
** After finding there was some activity on a listening fd
** a new socket is created in order to create a connexion with the client.
** This fd is then added to the active set of fds to be monitored
*/
void	addClient(int fd, fd_set *activeSet)
{
	int			sockCli;
	sockaddr_in	addrCli;
	socklen_t	addrCliLen = sizeof(addrCli);
	if ((sockCli = accept(fd, reinterpret_cast<sockaddr*>(&addrCli), &addrCliLen)) == ERROR) {
		closeExit("accept", activeSet);
	}
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Server: connection from " << inet_ntoa(addrCli.sin_addr);
	std::cout << " on port " << ntohs(addrCli.sin_port) << std::endl;
	std::cout << "-----------------------------" << std::endl;
	FD_SET(sockCli, activeSet);
}

/*
** After finding there was some activity on a client fd,
** the client has potentially:
** 1) Stopped the connexion
** 2) Sent a request
** this function manages those 2 cases (+ the case of a read error)
*/

void	readClient(int fd, fd_set *activeSet, fd_set *writeSet, std::map<int, std::string> &answer, std::vector<Server> server_g)
{
	// Do while loop to read <3
	char	buffer[4096];
	int		ret = read(fd, buffer, 4095);
	if (ret == ERROR) {
		closeExit("read", activeSet);
	}
	else if (ret == 0) {
		std::cout << "client " << fd << ": connection lost" << std::endl;
		close(fd);
		FD_CLR(fd, activeSet);
	}
	else {
		buffer[ret] = '\0';
		std::cout << "-----------------------------" << std::endl;
		std::cout << "Server: new message from ";
		std::cout << "client " << fd << ":" <<std::endl;
		std::cout << "-----------------------------" << std::endl;
		// buf needs to be sent to the parsing function
		std::cout << buffer << std::endl;
		FD_SET(fd, writeSet);
		//answer[fd] = the function that gets the output to give to the client
		// ex: answer[fd] = getAnswer(std::string(request));
		Request *req = requestParser(buffer, server_g);
		std::vector<Server>::iterator it = server_g.begin();
        for (; it != server_g.end(); it++)
        {
            if (vector_contains_str(it->getServerName(), req->getHost()))
                break;
        }
        Response res(*req, *it);
		answer[fd] = res.call();
		delete req;
	}
}

/*
** This function writes the answer previously calculated to the client
** after select() said that the write operation was legal
*/
void	writeClient(int fd, fd_set *activeSet, fd_set *writeSet, std::map<int, std::string> &answer)
{
	if (write(fd, answer[fd].c_str(), answer[fd].size()) == ERROR) {
		closeExit("write", activeSet);
	}
	answer.erase(fd);
	FD_CLR(fd, writeSet);
}

/*
** Starting function that takes Server instance as parameter
*/
void	sockServ(std::vector<Server> server_g)
{
	std::vector<int>	sockArr = sockListen(server_g[0].getPort());
	std::map<int, std::string>	answer;
	fd_set	activeSet, readSet, writeSet;
	FD_ZERO(&activeSet);
	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	for (size_t i = 0; i < sockArr.size(); ++i) {
		FD_SET(sockArr[i], &activeSet);
	}
	while (1) {
		readSet = activeSet;
		if (select(FD_SETSIZE, &readSet, &writeSet, NULL, NULL) == ERROR) {
			closeExit("select", &activeSet);
		}
		for (int fd = 0; fd < FD_SETSIZE; ++fd) {
			if (FD_ISSET(fd, &readSet)) {
				if (isListen(sockArr, fd)) {
					addClient(fd, &activeSet);
				}
				else {
					readClient(fd, &activeSet, &writeSet, answer, server_g);
				}
			}
			else if (FD_ISSET(fd, &writeSet)) {
				writeClient(fd, &activeSet, &writeSet, answer);
			}
		}
	}
}