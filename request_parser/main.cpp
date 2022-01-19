/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 16:34:02 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:51:10 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request_Parser.hpp"

int	main(void)
{
	requestParser("GET /wiki/Wikipedia:Accueil_principal HTTP/2\r\nHost: fr.wikipedia.org\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:96.0) Gecko/20100101 Firefox/96.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nReferer: https://www.google.com/\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nCache-Control: max-age=0");
// la ligne suivante est en fait un response header
//	requestParser("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccept-Encoding: chunked\r\n\r\n26\r\nVoici les données du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n");
}
