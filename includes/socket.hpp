/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:02:37 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/02 16:41:44 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <cstdlib>
# include <cstring>

# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/time.h>

# include "Response.hpp"
# include "Server.hpp"
# include "SockData.hpp"

void				closeListen(std::vector<int> sockListen, size_t endInd);
std::vector<int>	sockListen(std::vector<size_t>	port);
void				sockServ(std::vector<Server> servers, std::vector<size_t> ports);


#endif
