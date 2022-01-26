/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 18:42:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/26 17:40:35 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <ctime>
/**
void autoIndex(std::string path) {
	
	DIR	*dirStream;
	struct dirent	*nextEntry;
	struct stat		buf;

	dirStream = opendir(path.c_str());

	while ((nextEntry = readdir(dirStream)) != NULL) {
		std::string name(nextEntry->d_name);
		name = path + "/" + name;
		stat(name.c_str(), &buf);

		std::cout << nextEntry->d_name; // name
		if (S_ISDIR(buf.st_mode))
			std::cout << "/";
		std::cout << " ";
		std::string time = asctime(localtime(&buf.st_ctime));
		std::cout << time.substr(0, time.size() - 1); // last modif date
		std::cout << buf.st_size << std::endl; // size
	}

	closedir(dirStream);
	return ;
}
**/
std::string	autoIndexHeader(std::string path) {
	return "<!DOCTYPE html><html><head><title>" + path + "</title></head><body>";
}

std::string autoIndexPageTitle(std::string path) {
	return "<h1>Index of " + path + "/" + "</h1>";
}

std::string	autoIndexColumnNames() {
	return "<div style=\"float: left; width: 32%;\"><p><strong>File Name:</strong></p></div>" + "<div style=\"float: left; width: 32%;\"><p><strong>Last Opened:</strong></p></div>" + "<div style=\"float: left; width: 32%;\"><p><strong>Size (in octets):</strong></p></div>";
}

std::string	autoIndexDrawnLine() {
	return "<hr size=\"1\" width=\"100%\" color=\"black\">";
}

std::string	autoIndexVarName(std::string path, std::string name, struct stat buf) {
	std::string content;

	content = "<div style=\"float: left; width: 32%;\"><a href=\"";
	content += path;
	content += "\">";
	content += name;
	if (S_ISDIR(buf.st_mode))
		content += "/";
	content += "</a></div>";
	return content;
}

std::string autoIndexVarDate(struct stat buf) {
	
	std::string time;
   	
	time = asctime(localtime(&buf.st_ctime)); // get raw date (# of seconds since january 1970) into readable date
	time = time.substr(0, time.size() - 1); // removes the \n in the end

	return "<div style=\"float: left; width: 32%;\">" + time + "</div>";
}

std::string autoIndexVarSize(struct stat buf) {
		size_t				size = buf.st_size;
		std::stringstream	ss;
		std::string			putSize;
		size_t				giga = 1073741824;
		size_t				mega = 1048576;
		size_t				kilo = 1024;

		if (size >= giga) {
			size /= giga;
			ss << size;
			putSize = ss.str() + "G";
		}
		else if (size >= mega) {
			size /= mega;
			ss << size;
			putSize = ss.str() + "M";
		}
		else if (size >= kilo) {
			size /= kilo;
			ss << size;
			putSize = ss.str() + "K";
		}
		return "<div style=\"float: left; width: 32%;\">" + putSize + "</div>";
}

std::string	autoIndexBuilder(std::string path) {
	DIR				*dirStream;
	struct dirent	*nextEntry;
	struct stat		buf;
	std::string		content;

	dirStream = opendir(path.c_str());

	content = autoIndexHeader(path);
	content += autoIndexPageTitle(path);
	content += autoIndexColumnNames();
	content += autoIndexDrawnLine();

	while ((nextEntry = readdir(dirStream)) != NULL) {
		std::string name(nextEntry->d_name);
		path = path + "/" + name;
		stat(path.c_str(), &buf);

		content += autoIndexVarName(path, name, buf);
		content += autoIndexVarDate(buf);
		content += autoIndexVarSize(buf);
	}

	content += "</body></html>";
	closedir(dirStream);

	return content;
}
/**
int	main(void) {
	std::string path("/Users/pnielly/pnielly/viro");
	autoIndex(path);
}**/
