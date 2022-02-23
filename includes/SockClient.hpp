/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:39:50 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/23 03:46:53 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_CLIENT_HPP
# define SOCK_CLIENT_HPP

# include <string>
# include <vector>
# include <sstream>
# include <iostream>

# include "RequestParser.hpp"
# include "ResponseHeader.hpp"
# include "Response.hpp"

class SockClient {

	private:

		char			*ip_;
		size_t			port_;
		bool			chunk_;
		int				requestType_;
		std::string		tmpRequest_;
		std::string		finalRequest_;
		unsigned int	totalLength_;
		Request			request_;
		Response		response_;
		Server			server_;
		std::string		data_;

	public:
		SockClient(void);
		SockClient(const SockClient &sockClient);
		~SockClient(void);
		SockClient	&operator=(const SockClient &sockClient);

		/* setters */
		void			setIp(char *ip);
		void			setPort(size_t port);
		void			setChunk(bool chunk);
		void			setRequest(const Request &request);
		void			setResponse(const Response &response);
		void			setServer(const Server &server);
		void			setInputFd(int inputFd);
		void			setOutputFd(int outputFd);

		/* checkers */
		bool			isChunk(void) const;
		bool			isTmpRequestChunk(void) const;
		bool			isChunkEof(void) const;
		/* getters */
		char			*getIp(void) const;
		size_t			getPort(void) const;
		int				&getRequestType(void);
		std::string		&getTmpRequest(void);
		std::string		&getFinalRequest(void);
		unsigned int	&getTotalLength(void);
		Request			&getRequest(void);
		Response		&getResponse(void);
		Server			&getServer(void);
		std::string		&getData(void);

};

#endif
