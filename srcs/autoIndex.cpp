/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 18:42:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/03 19:47:38 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "autoIndex.hpp"
#include "Response.hpp"
#include "utils.hpp"

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *StatFailedException::what() const throw() { return "stat() failed in the autoIndexBuilder() function."; }

std::string	autoIndexHeader(std::string path) {
	return "<!DOCTYPE html><html><head><title>" + path + "</title></head><body>";
}

std::string autoIndexPageTitle(std::string path) {
	size_t end = path.find("..");

	path = (end != std::string::npos) ? path.substr(0, end - 1) : path.substr(0, end);
	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1);
	return "<h1>Index of " + path + "/" + "</h1>";
}

std::string	autoIndexColumnNames() {
	return "<div style=\"float: left; width: 32%;\"><p><strong>File Name:</strong></p></div>"
	"<div style=\"float: left; width: 32%;\"><p><strong>Last Opened:</strong></p></div>"
	"<div style=\"float: left; width: 32%;\"><p><strong>Size (in octets):</strong></p></div>";
}

std::string	autoIndexDrawnLine() {
	return "<hr size=\"1\" width=\"100%\" color=\"black\">";
}

/**
 * varName(): File Name
 * DATA_FOLDER is the folder's name where all data is stored (default: test)
**/
std::string	autoIndexVarName(std::string path, std::string name, struct stat buf, std::string generalRoot) {
	std::string content;
	size_t	pos;
	std::string	data_folder("/" + generalRoot);

	content = "<div style=\"float: left; width: 32%;\"><a href=\"";
	pos = path.rfind(data_folder) + data_folder.size();
	content += path.substr(pos, path.length()) + "/" + name;
	content += "\">";
	content += name;
	if (S_ISDIR(buf.st_mode))
		content += "/";
	content += "</a></div>";
	return content;
}

/**
 * VarDate(): Last Opened
**/
std::string autoIndexVarDate(struct stat buf) {
	
	std::string time;
   	
	time = asctime(localtime(&buf.st_ctime)); // get raw date (# of seconds since january 1970) into readable date
	time = time.substr(0, time.size() - 1); // removes the \n in the end

	return "<div style=\"float: left; width: 32%;\">" + time + "</div>";
}

/**
 * varSize(): Size (in octets)
**/
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
			putSize = ss.str() + "Go";
		}
		else if (size >= mega) {
			size /= mega;
			ss << size;
			putSize = ss.str() + "Mo";
		}
		else if (size >= kilo) {
			size /= kilo;
			ss << size;
			putSize = ss.str() + "Ko";
		}
		if (putSize == "")
			putSize = "<1Ko";
		return "<div style=\"float: right; width: 32%;\">" + putSize + "</div>";
}

/**
 * builder()
**/
std::string	Response::autoIndexBuilder(std::string path) {
	DIR				*dirStream;
	struct dirent	*nextEntry;
	struct stat		buf;
	std::string		content;
	std::string		newPath;

	dirStream = opendir(path.c_str()); // get list of files and directories

	content = autoIndexHeader(path);
	content += autoIndexPageTitle(path);
	content += autoIndexColumnNames();
	content += autoIndexDrawnLine();

	readdir(dirStream); // skip "."
	while ((nextEntry = readdir(dirStream)) != NULL) {
		std::string name(nextEntry->d_name);
		newPath = path + "/" + name;
		if (stat(newPath.c_str(), &buf) < 0) // get Date and Size
			throw StatFailedException();
		
		content += autoIndexVarName(path, name, buf, _generalRoot);
		content += autoIndexVarDate(buf);
		
		// ".." is a special case
		if (name == "..") {
			content += "<div style=\"float: right; width: 32%;\">-</div>";
		}
		else {
			content += autoIndexVarSize(buf);
		}
	}

	content += "</body></html>";
	closedir(dirStream);

	return content;
}
