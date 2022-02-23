/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:53 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/23 03:33:45 by fhamel           ###   ########.fr       */
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
# define NO_CGI 0
# define CGI 1

# define BUF_SIZE 4096

class SockData {
	
	public:
	
		typedef	std::pair<unsigned int, std::string>	unchunk_t;
	
	private:

		std::vector<Server>						servers_;
		std::vector<std::pair<int, size_t> >	sockListen_;
		std::map<int, int>						dataFds_;
		std::map<int, SockClient>				clients_;
		fd_set									activeSet_;
		fd_set									readSet_;
		fd_set									writeSet_;

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
		
		/* checkers */
		bool		isSockListen(int fd) const;
		bool		isSockClient(int fd) const;
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;
		bool		isReadingOver(int fd) const;

		/* getters */
		fd_set		*getReadSet(void);
		fd_set		*getWriteSet(void);
		size_t		getSizeListen(void) const;
		int			getSockListen(size_t index) const;

		/* client manager */
		void		addClient(int fd);
		void		recvClient(int fd);
		void		sendClient(int fd);

		/* requests */
		void		cgiInputFile(int fd, std::string strFd);
		void		cgiOutputFile(int fd, std::string strFd);
		void		setRequestType(int fd, Request &request, Server &server);
		void		setResponse(int fd);
		void		requestReceived(int fd);
		void		setInternalError(int fd);

		/* utils */
		void		recvClientClose(int fd, int ret);
		void		clearCgiFiles(int fd);
		void		clearClient(int fd);
		void		resetClient(int fd);
		void		closeListen(size_t endInd);
		void		modifyChunkRequest(int fd);
		unchunk_t	unchunk(std::string str);
		void		normalRequest(int fd);
		void		startChunkRequest(int fd);
		void		endChunkRequest(int fd);
		void		concatChunks(int fd);
		
		/* msg connection */
		void		cnxFailed(void);
		void		cnxRefused(SockClient sockClient);
		void		cnxAccepted(SockClient sockClient);

		/* closing connexion */
		void		cnxCloseRecv(int fd);
		void		cnxCloseSend(int fd);

		/* msg send */
		void		msgRecv(int fd);
		void		msgSent(int fd);

		/* errors */
		void		exceptionError(int fd, std::exception &e);
		void		systemFailure(std::string str, int fd);

		/* exceptions */
		struct badAllocException : public std::exception {
			const char	*what(void) const throw() { return "memory overload"; }
		};

		struct badChunkRequestException : public std::exception {
			const char	*what() const throw() { return "Chunk request is not ended by a \"\\r\\n\""; }
		};

		struct cgiException : public std::exception {
			const char	*what() const throw() { return "CGI exception"; }
		};

};

#endif
