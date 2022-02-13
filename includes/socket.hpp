/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:02:37 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/13 12:38:09 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <cstdlib>
# include <cstring>

# include "SockData.hpp"

void				sockServ(std::vector<Server> servers, std::vector<size_t> ports);


#endif
