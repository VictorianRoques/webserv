/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockExec.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 19:30:31 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/14 19:42:59 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_EXEC_HPP
# define SOCK_EXEC_HPP

# include <string>

class SockExec {
	
	private:

		int			fd_client_; // socket client -> write response to this fd
		int			fd_data_; // file/dir asked (ex: index.html) or -1 if CGI request
		int			fd_request_; // if CGI needed -> request in the file (pipe: fd[0])
		std::string	request_; // request in a string for parsing

	public:

		/* getters */
		int			getFdClient(void) const;
		int			getFdData(void)	const;
		int			getFdRequest(void) const;
		std::string	getRequest(void) const;
		
};

#endif
