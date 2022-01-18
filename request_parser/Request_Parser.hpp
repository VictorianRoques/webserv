/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_Parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 12:57:18 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/18 18:34:01 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include "../utils/utils.hpp"

void	requestParser(std::string rq);

class Request {

	/**
	 * method pointer:
	**/
	public:
		typedef	void	(Request::*methodPointer)(std::string str);

		// Coplian Class
	public:
		Request();
		~Request();
		Request(const Request &);
		Request& operator=(const Request &);

		// variables
	private:
		// request line
		std::string	_method;
		std::string _path;
		std::string	_protocolVersion;

		// request header
		size_t		_contentLength;
		std::string	_contentType;
		std::string	_acceptEncoding;

		// body (aka payload)
		vec_str		_body;

	public:
		void	requestParser(std::string rq);

		//getters
	public:
		std::string	getMethod();
		std::string	getPath();
		std::string	getProtocolVersion();

		size_t		getContentLength();
		std::string	getContentType();
		std::string	getAcceptEncoding();

		vec_str		getBody();

		//setters
	public:
		void	setMethod(std::string method);
		void	setPath(std::string path);
		void	setProtocolVersion(std::string protocolVersion);
	
		void	setContentLength(size_t contentLength);
		void	setContentType(std::string contentType);
		void	setAcceptEncoding(std::string acceptEncoding);

		void	setBody(vec_str body);

		// parsing
	public:

		void	requestLine(std::string line);
		void	headerLine(std::string line);
		void	bodyLine(std::string line);
		
		// utils
	public:
		void	print_request();
};

# endif
