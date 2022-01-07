/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 20:37:19 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/07 16:50:45 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <cstdlib> // std::atoi()
# include <utility> // std::make_pair
# include <string>
# include <fstream>
# include <vector>

/**
 * type simplification (makes the code 'lighter')
**/
typedef std::vector<std::string>	vec_str;

/**
 * ft_putvec(): prints out the vector v
**/
template < class T >
void	ft_putvec(std::vector<T> v, std::string separator) {
	typename std::vector<T>::iterator it = v.begin();
		
	for (; it != v.end(); it++)
		std::cout << "<" << *it << ">" << separator;
	std::cout << std::endl;
}

#endif
