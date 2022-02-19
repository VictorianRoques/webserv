/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:39:50 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/19 19:12:17 by fhamel           ###   ########.fr       */
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
		bool			dataReady_;
		std::string		tmpRequest_;
		std::string		finalRequest_;
		Request			request_;
		Response		response_;
		int				inputFd_;
		int				outputFd_;
		std::string		responseBody_;
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
		void			setDataReady(bool dataReady);
		void			setRequest(const Request &request);
		void			setResponse(const Response &response);
		void			setInputFd(int inputFd);
		void			setOutputFd(int outputFd);

		/* checkers */
		bool			isChunk(void) const;
		bool			isDataReady() const;
		bool			isTmpRequestChunk(void) const;
		bool			isChunkEof(void) const;
		bool			isDeleteRequest(void) const;
		bool			isBadRequest(void) const;
		/* getters */
		char			*getIp(void) const;
		size_t			getPort(void) const;
		std::string		&getTmpRequest(void);
		std::string		&getFinalRequest(void);
		Request			&getRequest(void);
		Response		&getResponse(void);
		std::string		&getResponseBody(void);
		std::string		&getData(void);
		int				&getInputFd(void);
		int				&getOutputFd(void);

};

#endif
