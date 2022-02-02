/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:39:50 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/02 19:17:52 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_CLIENT_HPP
# define SOCK_CLIENT_HPP

# include <string>

class SockClient {
	
	private:
		char		*ip_;
		size_t		port_;
		// size_t		maxSize_;
		std::string	request_;

	public:
		SockClient(void);
		~SockClient(void);
		SockClient	&operator=(const SockClient &sockClient);

		/* setters */
		void		setIp(char *ip);
		void		setPort(size_t port);

		/* getters */
		char		*getIp(void) const;
		size_t		getPort(void) const;
		std::string	&getRequest(void);
};

#endif
