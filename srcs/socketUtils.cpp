/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 23:51:21 by fhamel            #+#    #+#             */
/*   Updated: 2022/01/27 12:35:56 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socket.hpp"

/*
** If an error occurs, all the socket fds are closed before exit
*/
void	closeListen(std::vector<int> sockListen, size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen[i]);
	}
}

/*
** If an error occurs later in the monitoring of the client fds
** all the socket fds are closed before exit
*/
void	closeSet(fd_set *activeSet)
{
	for (int i = 0; i < FD_SETSIZE; ++i) {
		if (FD_ISSET(i, activeSet)) {
			close(i);
		}
	}
}

/*
** The combination of 3 instructions often found to elagate the code
*/
void	closeExit(std::string str, fd_set *activeSet) {
	perror(str.c_str());
	closeSet(activeSet);
	exit(EXIT_FAILURE);
}

/*
** Check whether the socket fd is a listening one or not
*/
bool	isListen(std::vector<int> sockArr, int fd) {
	for (size_t i = 0; i < sockArr.size(); ++i) {
		if (fd == sockArr[i]) {
			return true;
		}
	}
	return false;
}