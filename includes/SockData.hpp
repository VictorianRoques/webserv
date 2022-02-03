/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:53 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/03 13:45:51 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_DATA_HPP
# define SOCK_DATA_HPP

# include <vector>
# include <string>
# include <map>

# include <arpa/inet.h>

# include "Server.hpp"
# include "Request_Parser.hpp"
# include "Parser.hpp"
# include "Response.hpp"
# include "SockClient.hpp"

# define ERROR -1
# define BUF_SIZE 2048

class SockData {
	private:
		std::vector<Server>			servers_;
		std::vector<int>			sockListen_;
		std::map<int, std::string>	response_;
		std::map<int, SockClient>	clients_;
		// std::vector<int>			chunk_;
		fd_set						activeSet_;
		fd_set						readSet_;
		fd_set						writeSet_;

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
		void		initReadSet(void);
		void		initWriteSet(void);
		void		addActiveSet(int fd);
		void		setSockListen(std::vector<int> sockListen);
		void		setReadToActive(void);
		void		setResponse(int fd);
		/* checkers */
		bool		isSockListen(int fd) const;
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;
		bool		isChunkFd(int fd) const;
		bool		isChunkRequest(std::string request) const;
		/* getters */
		fd_set		*getReadSet(void);
		fd_set		*getWriteSet(void);
		size_t		getSizeListen(void) const;
		int			getSockListen(size_t index) const;
		/* client manager */
		void		addClient(int fd);
		void		readClient(int fd);
		void		writeClient(int fd);
		/* connexion */
		void		cnxFailed(void);
		void		cnxRefused(SockClient sockClient);
		void		cnxAccepted(SockClient sockClient);
		/* read */
		void		cnxCloseRead(int fd);
		void		cnxCloseRead2(int fd);
		void		cnxCloseWrite(int fd);
		/* write */
		void		msgRecv(int fd);
		void		msgSent(int fd);
};

#endif