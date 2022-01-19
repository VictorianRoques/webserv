/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_Parser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 12:57:18 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 17:28:22 by pnielly          ###   ########.fr       */
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
		std::string	_contentLength;
		std::string	_contentType;
		std::string	_acceptEncoding;
		std::string	_transferEncoding;
		std::string	_cacheControl;
		std::string	_connection;
		std::string	_host;

		bool		_chunked;
		std::string	_fullPath;

		// body (aka payload)
		vec_str		_body;

	public:
		void	requestParser(std::string rq);

		//getters
	public:
		std::string	getMethod(); // GET, POST, DELETE, etc
		std::string	getPath(); // part of the final uri (add the root of the relevant location context)
		std::string	getProtocolVersion(); // HTTP1.1

		std::string	getContentLength(); // indicates the size of the message body, in bytes, sent to the recipient.
		std::string	getContentType(); // indicates the original media type of the resource (prior to any content encoding applied for sending).
		std::string	getAcceptEncoding(); // indicates the content encoding (usually a compression algorithm) that the client can understand. The server uses content negotiation to select one of the proposal and informs the client of that choice with the Content-Encoding response header.
		std::string	getTransferEncoding(); // specifies the form of encoding used to safely transfer the payload body to the user. (ex.: Transfer-Encoding: chunked)
		std::string	getCacheControl(); // holds directives (instructions) — in both requests and responses — that control caching in browsers and shared caches (e.g. Proxies, CDNs).
		std::string	getConnection(); // controls whether the network connection stays open after the current transaction finishes. If the value sent is keep-alive, the connection is persistent and not closed, allowing for subsequent requests to the same server to be done.
		std::string	getHost(); // host and port number to which the request is being sent.

		vec_str		getBody();

		bool		getChunked();
		std::string	getFullPath();

		//setters
	public:
		void	setMethod(std::string method);
		void	setPath(std::string path);
		void	setProtocolVersion(std::string protocolVersion);
	
		void	setContentLength(std::string contentLength);
		void	setContentType(std::string contentType);
		void	setAcceptEncoding(std::string acceptEncoding);
		void	setTransferEncoding(std::string transferEncoding);
		void	setCacheControl(std::string cacheControl);
		void	setConnection(std::string connection);
		void	setHost(std::string host);

		void	setBody(vec_str body);

		void	setChunked(bool chunked);
		void	setFullPath(std::string fullPath);

		// parsing
	public:

		void	requestLine(std::string line);
		void	headerLine(std::string line);
		void	bodyLine(std::string line);
		
		// utils
	public:
		void	print_request();
		void	isChunked();
		void	buildFullPath();
};

# endif
