/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:01:55 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:15:38 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../utils/utils.hpp"

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
		~Location();
		Location(const Location &);
		Location&	operator=(const Location &);

	// directives
	private:
		std::string							_matchModifier;
		std::string							_locationMatch;
		std::string							_root;
		vec_str								_methods;
		vec_str								_errorPage;
		std::pair<size_t, std::string>		_redirection;
		std::string							_index;
		bool								_autoIndex;
		std::pair<std::string, std::string>	_cgiHandler;
		std::string							_uploadDest;

		// getters
	public:
		std::string 						getMatchModifier() const;
		std::string							getLocationMatch() const;
		std::string							getRoot() const;
		vec_str								getMethods() const;
		vec_str								getErrorPage() const; // all signals mentionned are redirected to the file contained in the last element
		std::pair<size_t, std::string>		getRedirection() const;
		std::string							getIndex() const;
		bool								getAutoIndex() const;
		std::pair<std::string, std::string>	getCgiHandler() const;
		std::string							getUploadDest() const;

		//setters
	public:
		void	setMatchModifier(std::string matchModifier);
		void	setLocationMatch(std::string locationMatch);
		void	setRoot(std::string root);
		void	setMethods(vec_str methods);
		void	setErrorPage(vec_str errorPage);
		void	setRedirection(std::pair<size_t, std::string> redirection);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoIndex);
		void	setCgiHandler(std::pair<std::string, std::string> cgiHandler);
		void	setUploadDest(std::string uploadDest);

		// parsing helpers
	public:
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMethods(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirErrorPage(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirAutoIndex(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirRedirection(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirIndex(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirCgiHandler(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirUploadDest(vec_str::iterator it, vec_str::iterator vend);

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

};

#endif
