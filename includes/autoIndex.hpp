/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/27 11:13:32 by viroques          #+#    #+#             */
/*   Updated: 2022/02/04 14:22:54 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTO_INDEX_H
# define AUTO_INDEX_H

# include <string>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <iostream>
# include <sstream>
# include <ctime>

class StatFailedException: public std::exception {
	public:
		virtual char const *what() const throw();
};


#endif
