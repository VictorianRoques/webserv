/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:53 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/18 00:49:10 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_DATA_HPP
# define SOCK_DATA_HPP

# include <vector>
# include <string>
# include <map>
# include <new>
# include <fstream>
# include <iostream>
# include <utility>
# include <sstream>

# include <unistd.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/socket.h>
# include <sys/time.h>

# include "Server.hpp"
# include "RequestParser.hpp"
# include "Parser.hpp"
# include "Response.hpp"
# include "SockClient.hpp"
# include "cgiHandler.hpp"

# define ERROR -1

# define CGI -1
# define STR_DATA -2

# define BUF_SIZE 1024

class SockData {
	private:
		std::vector<Server>						servers_;
		std::vector<std::pair<int, size_t> >	sockListen_;
		std::map<int, int>						dataFds_;
		std::map<int, SockClient>				clients_;
		fd_set									activeSet_;
		fd_set									readSet_;
		fd_set									writeSet_;

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
		void		setSockListen(std::vector<size_t> ports);
		void		initActiveSet(void);
		void		initReadSet(void);
		void		initWriteSet(void);
		void		addActiveSet(int fd);
		void		setReadToActive(void);
		void		setDataFd(int fd, Request &request, Server &server);
		void		setResponse(int fd);
		void		setInternalError(int fd);
		void		setBadRequest(int fd);
		
		/* checkers */
		bool		isSockListen(int fd) const;
		bool		isSockClient(int fd) const;
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;
		bool		isReadingOver(int fd) const;
		bool		isBeginPipeReady(int fd);
		bool		isEndPipeReady(int fd);
		bool		isFileRequest(int fd);

		/* getters */
		fd_set		*getReadSet(void);
		fd_set		*getWriteSet(void);
		size_t		getSizeListen(void) const;
		int			getSockListen(size_t index) const;

		/* client manager */
		void		addClient(int fd);
		void		recvClient(int fd);
		void		sendClient(int fd);

		/* client manager utils */
		/* requests */
		void		fileRequest(int fd);
		void		cgiRequest(int fd);
		void		strDataRequest(int fd);
		void		requestReceived(int fd);
		/* cgi */
		void		writeToCgi(int fd);
		void		setUpCgi(int fd);

		/* utils */
		void		recvClientClose(int fd, int ret);
		void		clearDataFd(int fd);
		void		clearClient(int fd);
		void		closeListen(size_t endInd);
		std::string	cleanBody(std::string &responseBody);
		
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
		void		openFailure500(int fd);
		void		openFailure400(int fd);
		void		openFailureData(int fd);
		void		pipeFailure(int fd);
		void		writeFailure(int fd);

		/* bad alloc exception */
		struct badAllocException : public std::exception {
			const char	*what(void) const throw() { return "memory overload"; }
		};
};

#endif
