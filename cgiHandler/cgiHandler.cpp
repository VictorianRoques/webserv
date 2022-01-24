#include "cgiHandler.hpp"

cgiHandler::cgiHandler(Request &req)
{
    _env["PATH_TRANSLATED"] = req.getFullPath().substr(req.getFullPath().rfind("/") + 1);
    _env["QUERY_STRING"] = req.getQueryString();
    _env["PATH_INFO"] = req.getFullPath();
    _env["REQUEST_METHOD"] = req.getMethod();
    _env["CONTENT_TYPE"] = req.getContentType();
    _env["CONTENT_LENGTH"] = req.getContentLength();
	_env["REDIRECT_STATUS"] = "200";
    _body = req.getBody();
}

char**        cgiHandler::envToString()
{
    char **strEnv = new char *[this->_env.size() + 1];
    std::map<std::string, std::string>::const_iterator it = _env.begin();
    std::map<std::string, std::string>::const_iterator ite = _env.end();
    int i = 0;
	std::cout << "DEVUT" << std::endl;
    while (it != ite)
    {
        std::string elem = it->first + "=" + it->second;
        strEnv[i] = new char[elem.size() + 1];
        strEnv[i] = strcpy(strEnv[i], (const char*)elem.c_str());
        it++;
        i++;
    }
	std::cout << "FIN" << std::endl;
    strEnv[i] = NULL;
    return strEnv;
}

std::string     cgiHandler::execute(std::string  scriptFilename)
{
    char        **envp;
    pid_t       pid;
    int         tmpStdIn = dup(STDIN_FILENO);
    int         tmpStdOut = dup(STDOUT_FILENO);
    std::string newBody = "";

    try {
        envp = envToString();
    }
    catch (std::bad_alloc &e)
    {
            std::cerr << RED << e.what() << NC << std::endl;
    }

    FILE*   fileIn = fopen("tmpIn", "w+");
    FILE*   fileOut = fopen("tmpOut", "w+");
    int     fdIn =  fileno(fileIn);
    int     fdOut = fileno(fileOut);
    int     status;

    write(fdIn, _body.c_str(), _body.size());
    lseek(fdIn, 0, SEEK_SET);
    pid = fork();
    if (pid == -1)
    {
        std::cerr << RED << "Fork crashed." << std::endl;
        return ("Status: 500\r\n\r\n");
    }
    else if (pid == 0)
    {
        dup2(fdIn, STDIN_FILENO);
        dup2(fdOut, STDOUT_FILENO);
        execve(scriptFilename.c_str(), NULL, envp);
        std::cerr << RED << "Something went wrong with execve." << NC << std::endl;
        write(STDOUT_FILENO, "Status: 500\r\n\r\n", 20);
    }
    else
    {
        char buffer[4096];
        wait(&status);
        lseek(fdOut, 0, SEEK_SET);
        int ret = 1;
        while (ret > 0)
        {
            memset(buffer, 0, 4096);
            ret = read(fdOut, buffer, 4096 - 1);
            newBody += buffer;
        }

    }

    dup2(tmpStdIn, STDOUT_FILENO);
    dup2(tmpStdOut, STDOUT_FILENO);
    fclose(fileIn);
    fclose(fileOut);
    close(fdIn);
    close(fdOut);
    for (int i = 0; envp[i]; i++)
        delete [] envp[i];
    delete[] envp;
	std::cout << "PRINT MAPPPPP\n" << std::endl;
	ft_putmap(_env, "\n");
    return newBody;
}

