/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockExec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 19:39:47 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/14 19:42:50 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "SockExec.hpp"

/* getters */
int	SockExec::getFdClient(void) const
	{ return fd_client_; }

int	SockExec::getFdData(void) const
	{ return fd_data_; }

int	SockExec::getFdRequest(void) const
	{ return fd_request_; }

std::string	SockExec::getRequest(void) const
	{ return request_; }
