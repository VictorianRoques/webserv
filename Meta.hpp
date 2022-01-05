/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Meta.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 12:17:19 by user42            #+#    #+#             */
/*   Updated: 2022/01/05 12:22:51 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef META_HPP
# define META_HPP

# include "Parser.hpp"

class Meta {
	//Coplian Class
	public:
		Meta();
		~Meta();
		Meta(const Meta &);
		Meta	&operator=(const Meta &);

	private:
		std::vector<Server>	_servers;
};

#endif
