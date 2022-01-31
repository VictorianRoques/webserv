/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 15:19:47 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/31 13:19:32 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

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
	m.insert(std::make_pair(first, second));
	return m;
}

/**
 * vector_contains_str(): returns "true" if vector contains str
**/
bool	vector_contains_str(vec_str v, std::string str) {
	vec_str::iterator it = v.begin();

	for (; it != v.end(); it++) {
		if (*it == str)
			return true;
	}
	return false;
}

// WrongPathFormatException()
char const *WrongPathFormatException::what() const throw() { return "Found a \"..\" not surrounded by '/' --> Wrong Path Format (check the root directives)."; }

/**
 * removeBackwards(): removes useless backward moves in a path. (ex.: "../webserv/test" --> "test" | "test/../test" --> "test")
 * called by dirRoot() in Location.cpp
**/
std::string removeBackwards(std::string path) {
	size_t pos;
	size_t pos2;
	size_t count;
	size_t i;

	while ((pos = path.find("..")) != std::string::npos) {
		if (pos == 0) {
			if (path.size() <= 3)
				return ("/");
			count = 1;
			while (path.find("../", 1) == 3) {
				count++;
				path.erase(0, 3);
			}
			i = -1;
			while (++i <= count) {
				pos2 = path.find("/");
				path.erase(0, pos2 + 1);
			}
		}
		else if (path.rfind("/", pos) != pos - 1 || path.find("/", pos) != pos + 2 || pos == 1) {
			throw WrongPathFormatException();
		}
		else {
			pos2 = pos + 3;
			pos -= 2;
			while (path[pos] != '/' && pos != 0)
				pos--;
			if (pos == 0)
				pos2 += 1;
			path.erase(pos, pos2 - pos - 1);
		}
	}
	return path;
}
