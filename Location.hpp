/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:01:55 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/07 16:50:54 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "utils.hpp"

class Location {
	// Coplian Class
	public:
		Location();
		~Location();
		Location(const Location &);
		Location&	operator=(const Location &);

	// directives
	private:
		std::string	_root;

		// getters
	public:
		std::string getRoot() const;

		//setters
	public:
		void	setRoot(std::string root);

		// utils
	public:
		void	print_loc();
};

#endif
