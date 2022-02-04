/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/27 11:13:32 by viroques          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2022/01/28 16:35:37 by pnielly          ###   ########.fr       */
=======
/*   Updated: 2022/02/03 18:14:47 by pnielly          ###   ########.fr       */
>>>>>>> 023d8b61fb6df7aa22c47eb361348f8d7113e465
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
