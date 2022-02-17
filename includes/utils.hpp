/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 20:37:19 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/10 19:27:30 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <cstdlib> // std::atoi() ; std::getenv()
# include <utility> // std::make_pair
# include <string>
# include <fstream>
# include <vector>
# include <map>
# include <cstring>
# include <algorithm> // find() (in Parser.cpp)
# include <time.h>
# include <sstream>

// PathIsfile Include
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>

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
typedef std::vector<std::string>			vec_str;
typedef std::map<std::string, std::string>	map_str;

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
 * ft_putmap(): prints out the map m
**/
template < class T, class U >
void	ft_putmap(std::map<T, U> m, std::string separator) {
	typename std::map<T, U>::iterator it = m.begin();

	for (; it != m.end(); it++)
		std::cout << "<" << it->first << " = " << it->second << ">" << separator;
	std::cout << std::endl;
}

/**
 * prototypes of utils
**/

vec_str 							ft_split(std::string line, std::string delim);
std::map<std::string, std::string>	ft_map_split(std::string line, std::string delim);
bool								vector_contains_str(vec_str v, std::string str);
std::string							removeBackwards(std::string path);
std::string							cleanSlash(std::string path);
std::string							getPWD();
int     							pathIsDirectory(std::string &path);
int									pathIsFile(std::string &path);
std::string							getTime();
std::string							sizeToString(size_t size);
std::string							hrefLocation(std::string location);
std::string							findRightPath(std::string path, std::string root, bool autoIndex, std::string index);
std::string							makePathAbsolute(std::string);
long long							maxBodyAtoi(std::string maxBodySize);
std::string							getFileContent(std::string path);

/**
 * prototype of testing
**/
void	print_test(); //in Parser.cpp to print all servers class

//Exception
class WrongPathFormatException: public std::exception {
	public:
		virtual char const *what() const throw();
};

class WrongFormatMaxBodySizeException: public std::exception {
	public:
		virtual char const *what() const throw();
};

class TooBigMaxBodySizeException: public std::exception {
	public:
		virtual char const *what() const throw();
};

class FileDisapearedException: public std::exception {
	public:
		virtual char const *what() const throw();
};

class FailedToOpenFileException: public std::exception {
	public:
		virtual char const *what() const throw();
};

class FailedToCloseFileException: public std::exception {
	public:
		virtual char const *what() const throw();
};

#endif
