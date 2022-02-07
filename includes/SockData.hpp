/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:53 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/07 02:25:33 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_DATA_HPP
# define SOCK_DATA_HPP

# include <vector>
# include <string>
# include <map>
# include <new>
# include <fstream>

# include <arpa/inet.h>
# include <sys/socket.h>

# include "Server.hpp"
# include "RequestParser.hpp"
# include "Parser.hpp"
# include "Response.hpp"
# include "SockClient.hpp"

# define ERROR -1
# define BUF_SIZE 1024

class SockData {
	private:
		std::vector<Server>			servers_;
		std::vector<int>			sockListen_;
		std::map<int, std::string>	response_;
		std::map<int, SockClient>	clients_;
		fd_set						activeSet_;
		fd_set						recvSet_;
		fd_set						sendSet_;

		std::string	red;
		std::string	green;
		std::string	blue;
		std::string white;

	public:
		SockData(void);
		SockData(const SockData &sockData);
		~SockData(void);
		SockData	&operator=(const SockData &sockData);
		/* setters */
		void		setServers(std::vector<Server> servers);
		void		initActiveSet(void);
		void		initRecvSet(void);
		void		initSendSet(void);
		void		addActiveSet(int fd);
		void		setSockListen(std::vector<int> sockListen);
		void		setRecvToActive(void);
		void		setResponse(int fd);
		void		setInternalError(int fd);
		/* checkers */
		bool		isSockListen(int fd) const;
		bool		isRecvSet(int fd) const;
		bool		isSendSet(int fd) const;
		bool		isChunkFd(int fd) const;
		bool		isChunkRequest(std::string request) const;
		/* getters */
		fd_set		*getRecvSet(void);
		fd_set		*getSendSet(void);
		size_t		getSizeListen(void) const;
		int			getSockListen(size_t index) const;
		size_t		clientsAlloc(void);
		/* client manager */
		void		addClient(int fd);
		void		recvClient(int fd);
		void		sendClient(int fd);
		/* client manager utils */
		void		recvClientClose(int fd, int ret);
		/* msg connection */
		void		cnxFailed(void);
		void		cnxRefused(SockClient sockClient);
		void		cnxAccepted(SockClient sockClient);
		/* msg recv */
		void		cnxCloseRecv(int fd);
		void		cnxCloseRecv2(int fd);
		void		cnxCloseSend(int fd);
		/* msg send */
		void		msgRecv(int fd);
		void		msgSent(int fd);
		/* msg exception */
		void		exceptionError(int fd, std::exception &e);
		/* utils */
		void		printBuffer(char buffer[BUF_SIZE]) const;
		/* bad alloc exception */
		struct badAllocException : public std::exception {
			const char	*what(void) const throw() { return "bad alloc"; }
		};
};

#endif
