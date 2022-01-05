/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Meta.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 12:25:32 by user42            #+#    #+#             */
/*   Updated: 2022/01/05 12:30:32 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Meta.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Meta::Meta() {}

Meta::~Meta() {}

Meta::Meta(const Meta &x) {
	*this = x;
}

Meta&	operator=(const Meta &x) {
	if (*this != x)
		_servers = x._servers;
}

