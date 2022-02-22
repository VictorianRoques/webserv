/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:01:55 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/22 13:35:53 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "utils.hpp"

class Location {

		/**
		 *  method pointer typedef: redirects to parsing helper functions (see below)
		 *  return type: size_t (number of elements iterated over in the vector... 
		 *  ...while calling the helper function)
		 *  params:
		 *  -it = current position in the vector
		 *  -vend = vector.end()
		**/
	public:
		typedef size_t	(Location::*methodPointer)(vec_str::iterator it, vec_str::iterator vend);

	// Coplian Class
	public:
		Location();
		Location(size_t maxBodySize); //init
		~Location();
		Location(const Location &);
		Location&	operator=(const Location &);

	// directives
	private:
		std::string							_matchModifier; // always empty in this project
		std::string							_locationMatch;
		std::string							_root;
		vec_str								_methods;
		std::pair<size_t, std::string>		_redirection;
		std::string							_index;
		bool								_autoIndex;
		std::pair<std::string, std::string>	_cgiHandler;
		std::string							_uploadDest;
		long long							_maxBodySize;
	
		std::string							_generalRoot;
		bool								_rootSpecified; // true if found a root directive in the location

		// getters
	public:
		std::string 						getMatchModifier() const;
		std::string							getLocationMatch() const;
		std::string							getRoot() const;
		vec_str								getMethods() const;
		std::pair<size_t, std::string>		getRedirection() const;
		std::string							getIndex() const;
		bool								getAutoIndex() const;
		std::pair<std::string, std::string>	getCgiHandler() const;
		std::string							getUploadDest() const;
		long long							getMaxBodySize() const;

		std::string							getGeneralRoot() const;
		
		//setters
	public:
		void	setMatchModifier(std::string matchModifier);
		void	setLocationMatch(std::string locationMatch);
		void	setRoot(std::string root);
		void	setMethods(vec_str methods);
		void	setRedirection(std::pair<size_t, std::string> redirection);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoIndex);
		void	setCgiHandler(std::pair<std::string, std::string> cgiHandler);
		void	setUploadDest(std::string uploadDest);
		void	setMaxBodySize(long long maxBodySize);
		void	setMaxBodySize(std::string maxBodySize);
		
		void	setGeneralRoot(std::string generalRoot);

		// parsing helpers
	public:
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMethods(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirAutoIndex(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirRedirection(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirIndex(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirCgiHandler(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirUploadDest(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend);
		size_t	locationContext(vec_str::iterator it);
		void	locationChecker();

		// utils
	public:
		void	print_loc();

		// EXCEPTIONS

		class WrongMethodException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
		
		class WrongValue_AutoIndexException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
		
		class NonValidRedirectionException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class NonValidRootException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class NonValidIndexException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class NonValidCgiHandlerException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class WrongPathException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class MissingBracketException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class RootAndAbsolutePathException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class AutoIndexWithoutIndexException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

		class WrongLocationMatchException: public std::exception {
			public:
				virtual char const *what() const throw();
		};

};

#endif
