/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockExec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 19:39:47 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/16 16:39:02 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "SockExec.hpp"

SockExec::SockExec(void)
	{ return ; }

SockExec::SockExec(const SockExec &sockExec)
	{ *this = sockExec; }

SockExec::~SockExec(void)
	{ return ; }

SockExec	&SockExec::operator=(const SockExec &sockExec)
{
	dataFd_ = sockExec.dataFd_;
	clientFd_ = sockExec.clientFd_;
	return *this;
}

/* setters */
void	SockExec::setDataFd(int fd)
	{ dataFd_ = fd; }

void	SockExec::setClientFd(int fd)
	{ clientFd_ = fd; }

/* getters */
int	SockExec::getDataFd(void) const
	{ return dataFd_; }

int	SockExec::getClientFd(void) const
	{ return clientFd_; }

