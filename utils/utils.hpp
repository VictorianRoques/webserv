/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 20:37:19 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:51:27 by pnielly          ###   ########.fr       */
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
# include <map>

/**
 * COLORS
**/
# define BLACK "\e[1;30m"
# define RED "\e[1;31m"
# define GREEN "\e[1;32m"
# define YELLOW "\e[1;33m"
# define PURPLE "\e[1;34m"
# define MAGENTA "\e[1;35m"
# define CYAN "\e[1;36m"
# define WHITE "\e[1;37m"
# define NC "\e[0m" // NC = NO COLOR (reset)

# define COLOR_SERV CYAN
# define COLOR_LOC 	PURPLE
# define COLOR_REQ 	YELLOW

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

/**
 * prototypes of utils
**/
vec_str 							ft_split(std::string line, std::string delim);
std::map<std::string, std::string>	ft_map_split(std::string line, std::string delim);
size_t								str_match_size(std::string a, std::string b);

/**
 * prototypes of main functions
**/
void	requestParser(std::string rq);

#endif
