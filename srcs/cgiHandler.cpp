#include "cgiHandler.hpp"

cgiHandler::cgiHandler(Request &req)
{
    _env["PATH_TRANSLATED"] = req.getFullPath();
    _env["QUERY_STRING"] = req.getQueryString();
    _env["PATH_INFO"] = req.getFullPath().substr(req.getFullPath().rfind("/") + 1);
    _env["REQUEST_METHOD"] = req.getMethod();
    _env["CONTENT_TYPE"] = req.getContentType();
    _env["CONTENT_LENGTH"] = req.getContentLength();
	_env["REDIRECT_STATUS"] = "200";
    _body = req.getBody();
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

int    cgiHandler::upload(std::string uploadDest)
{
    size_t foundFileName = _body.find("filename=");
    if (foundFileName == std::string::npos)
        return (1);
    
    std::string fileName = _body.substr(foundFileName, _body.find("\n", foundFileName));
    fileName = fileName.substr(0, fileName.find("\n"));
    fileName = fileName.substr(10);
    fileName.erase(fileName.length() - 2);
    uploadDest = uploadDest + "/" + fileName;

    std::string boundary = _env["CONTENT_TYPE"].substr(_env["CONTENT_TYPE"].find("boundary=") + 9);
    _body = _body.substr(_body.find("\r\n\r\n") + 4);
    _body = _body.substr(0, _body.find(boundary));
    _body = _body.substr(0, _body.length() - 2);

    std::ofstream myfile;
    myfile.open(uploadDest.c_str(), std::ofstream::out | std::ofstream::binary);
    if (myfile.is_open() == false)
        return (1);
    myfile << _body.c_str();
    myfile.close();
    return (0);
}

std::string     cgiHandler::execute(std::string  pathToBinaryCgi)
{
    char        **envp;
    char        **argv;
    pid_t       pid;
    int         tmpStdIn = dup(STDIN_FILENO);
    int         tmpStdOut = dup(STDOUT_FILENO);
    std::string newBody;

    try {
        envp = envToString();
        argv = keyMapConvert(_env["PATH_TRANSLATED"]);
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
        return ("<html><body>FATAL ERROR</body></html>");
    }
    else if (pid == 0)
    {
        dup2(fdIn, STDIN_FILENO);
        dup2(fdOut, STDOUT_FILENO);
        execve(pathToBinaryCgi.c_str(), argv, envp);
        std::cerr << RED << "Something went wrong with execve." << NC << std::endl;
        write(STDOUT_FILENO, "<html><body>FATAL ERROR</body></html>", 38);
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
    remove("tmpIn");
    remove("tmpOut");
    for (int i = 0; envp[i]; i++)
        delete [] envp[i];
    delete[] envp;
    return newBody;
}

