#include "Parser.hpp"
#include "socket.hpp"

#define PORT 8080

// int main(int ac, char **av)
// {
//     Parser parser;

//     if (ac != 2)
//         std::cout << RED << "Error: " << NC << "Need one and only one argument\n";
//     else
//     {
//         try
//         {
//             parser.tokenizer(av);
//             // TESTING
//             parser.print_test();
//             //			requestParser(request_header, servers_g);
//         }
//         catch (std::exception &e)
//         {
//             std::cout << RED << "Error: " << NC << e.what() << std::endl;
//         }

//         std::vector<Server> server_g = parser.getServersG();

//         int server_fd, new_socket;
//         long valread;
//         struct sockaddr_in address;
//         int addrlen = sizeof(address);

//         // Response toto;
//         // std::string res = toto.getMethod();
//         // char hello[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
//         // Creating socket file descriptor
//         if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//         {
//             perror("In socket");
//             exit(EXIT_FAILURE);
//         }
//         int enable = 1;
//         if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
//         {
//             perror("In socket");
//             exit(EXIT_FAILURE);
//         }
//         address.sin_family = AF_INET;
//         address.sin_addr.s_addr = INADDR_ANY;
//         address.sin_port = htons(PORT);

//         memset(address.sin_zero, '\0', sizeof address.sin_zero);

//         if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
//         {
//             perror("In bind");
//             exit(EXIT_FAILURE);
//         }
//         if (listen(server_fd, 10) < 0)
//         {
//             perror("In listen");
//             exit(EXIT_FAILURE);
//         }

//         while (1)
//         {
//             printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//             if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
//             {
//                 perror("In accept");
//                 exit(EXIT_FAILURE);
//             }
//             char buffer[30000] = {0};
//             valread = read(new_socket, buffer, 30000);
//             // RESPONSE CREATION
//             std::cout << "AVANT REQUEST" << std::endl;
//             Request *req = requestParser(buffer, server_g);
//             std::cout << "POST REQUEST" << std::endl;
//             std::vector<Server>::iterator it = server_g.begin();
//             for (; it != server_g.end(); it++)
//             {
//                 if (vector_contains_str(it->getServerName(), req->getHost()))
//                     break;
//             }
//             Response res(*req, *it);
//             std::string s_res = res.call();
//             delete req;
//             printf("%s\n", buffer);
//             write(new_socket, s_res.c_str(), s_res.length());
//             printf("------------------Hello message sent-------------------\n");
//             close(new_socket);
//         }
//     }
//     return (0);
// }

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
            parser.print_test();
            //			requestParser(request_header, servers_g);
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