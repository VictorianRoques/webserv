#include "Parser.hpp"
#include "socket.hpp"

#define PORT 8080


int     main(int ac , char **av)
{

    Parser parser;

    if (ac != 2)
        std::cout << RED << "Error: " << NC << "Need one and only one argument\n";
    else
    {
        try
        {
            parser.tokenizer(av);
            // TESTING
            parser.print();
        }
        catch (std::exception &e)
        {
            std::cout << RED << "Error: " << NC << e.what() << std::endl;
            return (1);
        }

        std::vector<Server> server_g = parser.getServersG();
        std::vector<size_t> ports_g = parser.getPortsG();
        sockServ(server_g, ports_g);
    }
    return (0);
}
