/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 14:44:58 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/21 16:33:46 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER
# define REQUEST_PARSER

# include "utils.hpp"
# include "Location.hpp"
# include "Server.hpp"

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
		std::string	_secFetchDest;

		bool		_chunked;
		bool		_tooBig;
		std::string	_fullPath;
		std::string	_queryString;
		std::string	_uploadDest;

		// body (aka payload)
		std::string	_body;

		std::string	_generalRoot; 	// root of location / (without pwd)
		size_t		_redirCode;			// redirection

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
		std::string	getSecFetchDest(); // gives type of data ('document' or 'image')

		std::string	getBody();

		bool		getChunked();
		bool		getTooBig();
		std::string	getFullPath();
		std::string	getQueryString();
		std::string	getUploadDest();
		std::string	getGeneralRoot();
		size_t		getRedirCode();

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
		void	setSecFetchDest(std::string secFetchDest);

		void	setBody(std::string body);

		void	setChunked(bool chunked);
		void	setTooBig(bool tooBig);
		void	setFullPath(std::string fullPath);
		void	setQueryString(std::string queryString);	
		void	setUploadDest(std::string uploadDest);	
		void	setGeneralRoot(std::string generalRoot);
		void	setRedirCode(size_t redirCode);

		// parsing
	public:

		void	firstLine(std::string line);
		void	headerLine(std::string line);
		void	bodyLine(std::string line);
		
		// utils
	public:
		void		print_request();
		void		isChunked();
		void		buildFullPath(Location loc);
		Server		findRightServer(std::vector<Server> servers_g, Request *request);
		Location	findRightLocation(std::vector<Location> loc, Request *request);
		void		queryString();


		//EXCEPTIONS
		class NoHostException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

};

Server 								findRightServer(std::vector<Server> servers_g, Request request);
Location 							findRightLocation(std::vector<Location> loc, Request req);

# endif
