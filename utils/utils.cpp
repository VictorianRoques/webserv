/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 15:19:47 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:51:29 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

/**
 * ft_split(): splits string into a vec_str
 * any character of delim is a delimiter
**/
vec_str	ft_split(std::string line, std::string delim) {
	vec_str	v;
	size_t	pos;
	size_t	posend;

	while (line.find_first_not_of(delim) != std::string::npos) {
		pos = line.find_first_not_of(delim);
		posend = std::min(line.find_first_of(delim, pos), line.length());
		v.push_back(line.substr(pos, posend - pos));
		line.erase(0, posend);
	}
	return v;
}

/**
 * ft_map_split(): splits string into pair <std::string, std::string>
**/
std::map<std::string, std::string>	ft_map_split(std::string line, std::string delim) {
	std::map<std::string, std::string>	m;

	std::string first = line.substr(0, line.find(delim) - 1);
	std::string second = line.substr(line.find(delim) + 1, std::string::npos);
	m.insert(std::make_pair<std::string, std::string>(first, second));
	return m;
}

/**
 * str_match_size(): returns the number of matching characters between two strings
**/
size_t	str_match_size(std::string a, std::string b) {
	size_t	i = 0;

	while (!a.empty() && !b.empty() && a[0] == b[0]) {
		i++;
		a++;
		b++;
	}
	return (i);
}
