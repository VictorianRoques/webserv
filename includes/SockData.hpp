/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:53 by fhamel            #+#    #+#             */
/*   Updated: 2022/01/30 20:52:59 by fhamel           ###   ########.fr       */
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

# define ERROR -1

class SockData {
	private:
		std::vector<Server>			servers_;
		std::vector<int>			sockListen_;
		std::map<int, std::string>	answer_;
		fd_set						activeSet_;
		fd_set						readSet_;
		fd_set						writeSet_;

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
		/* checkers */
		bool		isSockListen(int fd) const;
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;
		/* getters */
		fd_set		*getReadSet(void);
		fd_set		*getWriteSet(void);
		size_t		getSizeListen(void) const;
		int			getSockListen(size_t index) const;
		/* client manager */
		void		addClient(int fd);
		void		readClient(int fd);
		void		writeClient(int fd);
};

#endif