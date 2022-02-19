/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:17:37 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/19 22:23:21 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgiHandler.hpp"

cgiHandler::cgiHandler() {}

cgiHandler::cgiHandler(Request &req)
{
    /* Serveur */
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SCRIPT_NAME"] = "cgi_binary/darwin_phpcgi";
    _env["SERVER_NAME"] = "localhost";
    _env["SERVER_PORT"] = "8080";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";

    /* Requete */
    _env["REDIRECT_STATUS"] = "200";
    _env["REQUEST_METHOD"] = req.getMethod();
    _env["CONTENT_TYPE"] = req.getContentType();
    _env["CONTENT_LENGTH"] = req.getContentLength();
    _env["PATH_TRANSLATED"] = req.getFullPath();
    _env["PATH_INFO"] = req.getFullPath().substr(req.getFullPath().rfind("/") + 1);
    _env["QUERY_STRING"] = req.getQueryString();

    _body = req.getBody();
}

cgiHandler::cgiHandler(const cgiHandler &cgi) { *this = cgi; }

cgiHandler::~cgiHandler() {}

cgiHandler&     cgiHandler::operator=(const cgiHandler &cgi)
{
    _env = cgi._env;
    _body = cgi._body;
    return *this;
}

char**        cgiHandler::envToString()
{
    char **strEnv = new char *[_env.size() + 1];
    std::map<std::string, std::string>::const_iterator it = _env.begin();
    std::map<std::string, std::string>::const_iterator ite = _env.end();
    int i = 0;
    while (it != ite)
    {
        std::string elem = it->first + "=" + it->second;
        strEnv[i] = new char[elem.size() + 1];
        strEnv[i] = strcpy(strEnv[i], elem.c_str());
        it++;
        i++;
    }
    strEnv[i] = NULL;
    return strEnv;
}

char**          cgiHandler::keyMapConvert(std::string key)
{
    char **argv = new char *[2];
    argv[0] = new char[key.size() + 1];
    argv[0] = strcpy(argv[0], key.c_str());
    argv[1] = NULL;
    return argv;
}

int             cgiHandler::startCgi(int fd)
{
    char        **envp;
    char        **argv;
    pid_t       pid;
    int         status;

    envp = envToString();
    argv = keyMapConvert(_env["PATH_TRANSLATED"]);
    /* write in getDataFd() with body ! */
    pid = fork();
    if (pid == -1)
    {
        std::cerr << RED << "Fork crashed." << std::endl;
        return (-1);
    }
    else if (pid == 0)
    {
        std::stringstream	ss;
		ss << fd;
		std::string	pathFileIn = "./cgi_binary/.cgi_input_" + ss.str();
        std::string pathFileOut = "./cgi_binary/.cgi_output_" + ss.str();
		int inputFd, outputFd;
        if ((inputFd = open(pathFileIn.c_str(),
        O_RDONLY)) == -1) {
            std::cerr << RED << "CGI: error: opening [.cgi_input]" << NC << std::endl;
            exit(EXIT_FAILURE);
        }
        if ((outputFd = open(pathFileOut.c_str(),
        O_CREAT | O_TRUNC | O_WRONLY, 0666)) == -1) {
            std::cerr << RED << "CGI: error: opening [.cgi_output]" << NC << std::endl;
            exit(EXIT_FAILURE);
        }
        if (dup2(inputFd, STDIN_FILENO) == -1) {
            std::cerr << RED << "CGI: error: dup2" << NC << std::endl;
            exit(EXIT_FAILURE);
        }
        close(inputFd);
        if (dup2(outputFd, STDOUT_FILENO) == -1) {
            std::cerr << RED << "CGI: error: dup2" << NC << std::endl;
            exit(EXIT_FAILURE);
        }
        close(outputFd);
        if (execve("cgi_binary/darwin_phpcgi", argv, envp) == -1) {
            std::cerr << RED << "CGI: error: execve" << NC << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    wait(&status);
    if (status != 0) {
        std::cerr << RED << "status: " << status << " | CGI: abort" << NC << std::endl;
        return -1;
    }
    for (int i = 0; envp[i]; i++)
        delete [] envp[i];
    delete[] envp;
    return (0);
}

