/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 18:17:41 by viroques          #+#    #+#             */
/*   Updated: 2022/02/08 20:55:05 by viroques         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HEADER_H
# define RESPONSE_HEADER_H

# include "utils.hpp"

class ResponseHeader
{

public:

    ResponseHeader();
    std::string     getStatus();
    std::string     getContentType();
    size_t          getBodyLength();
    std::string     getDate();
    std::string     getLocation();
    std::string     getHeader();

    void                setStatus(std::string code);
    void                setContentType(std::string contentType);
    void                setBodyLength(size_t length);
    // void                setDate();
    // void                setLocation(std::string location);
    void                setHeader(std::string status, std::string contentType, size_t length);
    void                setStatusError(std::string code,size_t length);
    void                setStatusRedirect(std::string code, size_t length);
    void                setCgiHeader(std::string cgiHeader);
    void                writeHeader();

private:

    std::string _status;
    std::string _contentType;
    size_t      _bodyLength;
    std::string _date;
    std::string _location;
    std::string _header;
};

#endif
