/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockExec.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 19:30:31 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/16 14:19:36 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCK_EXEC_HPP
# define SOCK_EXEC_HPP

# include <string>

class SockExec {
	
	private:

		int			dataFd_; // dup2 stdin this fd
		int			clientFd_; // dup2 stdout this fd

	public:

		SockExec(void);
		SockExec(const SockExec &sockExec);
		~SockExec(void);
		SockExec	&operator=(const SockExec &sockExec);

		/* setters */
		void		setDataFd(int fd);
		void		setClientFd(int fd);

		/* getters */
		int			getDataFd(void) const;
		int			getClientFd(void) const;
		
};

#endif
