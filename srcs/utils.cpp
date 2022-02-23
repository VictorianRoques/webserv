/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 15:19:47 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/23 17:41:42 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

#define MAX_LONG_LONG 9223372036854775807

// EXCEPTIONS
char const *WrongPathFormatException::what() const throw() { return "Found a \"..\" not surrounded by '/' --> Wrong Path Format (check the root directives)."; }
char const *WrongFormatMaxBodySizeException::what() const throw() { return "Wrong Format of client_max_body_size. Your value is probably too big or negative. Maybe you put several values.\nEx.: client_max_body_size 30M\nAllowed magnitudes : K, M, G"; }
char const *TooBigMaxBodySizeException::what() const throw() { return "client_max_body_size is too big in config file."; }
char const *FileDisapearedException::what() const throw() { return "The file existed when the config file was parsed but now it just disapeared..."; }
char const *FailedToOpenFileException::what() const throw() { return "Failed to open file."; }
char const *FailedToCloseFileException::what() const throw() { return "Failed to close file."; }

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

/**
 * cleanSlash(): removes duplicate slashes in path
**/
std::string	cleanSlash(std::string path) {
	size_t pos;

	while ((pos = path.find("//")) != std::string::npos)
		path.erase(pos, 1);
	return path;
}

/**
 * getPWD(): returns a string equivalent to the return of 'pwd' command
**/
std::string	getPWD() {
	const char *env_pwd = std::getenv("PWD");
	std::string	pwd(env_pwd);
	return pwd;
}

/**
 * pathIsDirectory(): returns 1 if path is directory, else 0
**/
int     pathIsDirectory(std::string &path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
		if (s.st_mode & S_IFDIR)
			return 1;
	return 0;
}

int			pathIsFile(std::string &path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
		if (s.st_mode & S_IFREG)
			return 1;
	return 0;
}

std::string		getTime()
{
	char buf[100];
  	time_t now = time(0);
  	struct tm tm = *gmtime(&now);
  	strftime(buf, 100, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	std::string date(buf);
	return date;
}

std::string		sizeToString(size_t size)
{
	std::ostringstream convert;
	convert << size;
	return convert.str();
}

std::string		hrefLocation(std::string location)
{
	std::string ret = "<html><body>Your file has been upload! (no Cgi)";
	ret += "<a href=\"";
	ret += location;
	ret += "\"> Check Here </a>";
	ret += "</body></html>";
	return ret;
}

/**
 * findRightPath():
 * if relative path exists --> relative path
 * else if absolute path exists --> absolute path
 * else --> relative path (which will lead to error code 404)
**/
std::string	findRightPath(std::string path, std::string root, bool autoIndex, std::string index) {

	path = path.substr(0, path.find("?"));
	
	// case 1: autoIndex: off
	if (autoIndex == false) {
	std::string relative2 = cleanSlash(root + "/" + path);
		if (pathIsFile(relative2))
			return relative2;
		else if (pathIsDirectory(relative2)) {
			std::cout << "is directory" << std::endl;
			relative2 += "/" + index;
			return relative2;
		}
	}

	// case 2: path is relative
	std::string relative = cleanSlash(root + "/" + path);
	if (pathIsFile(relative) || pathIsDirectory(relative)) {
		return relative;
	}

	// case 3: path is absolute
	if (pathIsFile(path) || pathIsDirectory(path)) {
		return path;
	}

	return relative;
}

/**
 * makePathAbsolute(): relative path becomes absolute
**/
std::string	makePathAbsolute(std::string path) {
	std::string pwd = getPWD();
	size_t pos = std::min(path.find("/", 1), path.length());
	std::string start_path = path.substr(0, pos);
	
	pos = std::min(pwd.find(start_path), pwd.length());
	path = pwd.substr(0, pos) + "/" + path;

	return cleanSlash(path);
}

/**
 * maxBodyAtoi(): gets maxBodySize string and turns it into an int
**/
long long maxBodyAtoi(std::string maxBodySize) {
	size_t pos;
	size_t pos2;
	long long	ret;
	size_t K = 1000;
	size_t M = K * K;
	size_t G = K * M;
	std::stringstream s;

	pos = maxBodySize.find_first_of("0123456789");
	pos2 = maxBodySize.find_first_not_of("0123456789", pos);
	if (pos2 - pos > 18)
		throw TooBigMaxBodySizeException();

//	if (ret > MAX_LONG_LONG)
//		throw TooBigMaxBodySizeException();

//	ret = static_cast<long long>(std::atoi(maxBodySize.c_str()));
	s.clear();
	s << maxBodySize;
	s >> ret;
	
	pos = maxBodySize.length() - 1; //last element of maxSizeBody

	if (maxBodySize.at(pos) == ';')
		maxBodySize.erase(pos, 1);

	pos = maxBodySize.length() - 1; //last element of maxSizeBody

	if (maxBodySize.at(pos) == 'o')
		maxBodySize.erase(pos, 1);

	pos = maxBodySize.length() - 1; //last element of maxSizeBody

	if (maxBodySize.at(pos) == 'K' || maxBodySize.at(pos) == 'k')
		ret *= K;
	else if (maxBodySize.at(pos) == 'M' || maxBodySize.at(pos) == 'm')
		ret *= M;
	else if (maxBodySize.at(pos) == 'G' || maxBodySize.at(pos) == 'g')
		ret *= G;
	else if (!isdigit(maxBodySize.at(pos)))
		throw WrongFormatMaxBodySizeException();

	if (pos && !isdigit(maxBodySize.at(pos - 1)))
		throw WrongFormatMaxBodySizeException();

	if (maxBodySize.find_first_of("0123456789") != 0)
		throw WrongFormatMaxBodySizeException();
	
	if (!ret)
		throw WrongFormatMaxBodySizeException();

		return ret;
}

/**
 * getFileContent(): opens and reads a file and returns the content as a string
**/
std::string	getFileContent(std::string path) {
	std::ifstream	file;
	std::string		line;
	std::string		ret;

	if (!pathIsFile(path))
		throw FileDisapearedException();

	file.open(path.c_str());
	if (!file.is_open())
		throw FailedToOpenFileException();

	while (!file.eof()) {
		getline(file, line);
		ret += line;
	}

	file.close();
	return ret;
}

