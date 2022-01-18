/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 16:34:02 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/18 17:35:26 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request_Parser.hpp"

int	main(void)
{
	requestParser("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccept-Encoding: chunked\r\n\r\n26\r\nVoici les données du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n");
}
