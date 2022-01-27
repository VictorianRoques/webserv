/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:02:37 by fhamel            #+#    #+#             */
/*   Updated: 2022/01/27 12:34:46 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP

# define SOCKET_HPP

# include <iostream>
# include <cstdlib>
# include <cstring>
# include <vector>
# include <map>

# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/time.h>

# include "Request_Parser.hpp"
# include "Response.hpp"
# include "Parser.hpp"
# include "Server.hpp"

# define ERROR -1

// # include "Server.hpp"

/*
** Dummy class for tests
*/
// class Server {
// 	private:
// 		std::vector<size_t>	_port;
	
// 	public:
// 		std::vector<size_t> getPort(void) const
// 			{ return _port; }

// 		void	setPort(std::vector<size_t> port)
// 			{ _port = port; }
// };

void				closeListen(std::vector<int> sockListen, size_t endInd);
std::vector<int>	sockListen(std::vector<size_t>	port);
void				closeSet(fd_set *activeSet);
void				closeExit(std::string str, fd_set *activeSet);
bool				isListen(std::vector<int> sockArr, int fd);
void				addClient(int fd, fd_set *activeSet);
void				readClient(int fd, fd_set *activeSet,
					fd_set *writeSet, std::map<int, std::string> &answer,
					std::vector<Server> server_g);
void				writeClient(int fd, fd_set *activeSet,
					fd_set *writeSet, std::map<int, std::string> &answer);
void				sockServ(std::vector<Server> server_g);


#endif