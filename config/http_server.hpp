
#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "../Utils.h"
# include "../HTTP_request/HTTP_request.hpp"
# include "../HTTP_response/HTTP_response.hpp"
# include "server.hpp"
# include "location.hpp"

class   http_server{

    std::vector<struct pollfd> _Pfds;
    std::vector<server> _Servers;
    std::map<int, struct sockaddr_in> _Clients;
    std::map<int, HTTP_Request> _Requests;
	std::map<int, HTTP_Response> _Responses;
    //int _Servers_n

    public:
        http_server( void );
        ~http_server();

        void    parse(std::ifstream &config);
        server  parse_server(std::ifstream &config);
		void	parse_location(std::ifstream &config, server &Server);

        void    startServers( void );
        server  findServer( void );

        void    addNewFd(struct pollfd pfd);
        void    DeleteFd(struct pollfd pfd);
        void    UpdateFd(struct pollfd pfd, int i);
        bool    PfdExist(struct pollfd pfd);

        struct pollfd *pollFdTable( void ); 
        bool           FdExist(int fd);
        
        void    DirectRequest(HTTP_Request & req, struct sockaddr_in client, HTTP_Response & res);

        bool    isServer(int socket);
        bool    AlreadyBind(server & _server);

        const std::vector<server> get_servers( void ) const {
            return (_Servers);
        };

        void    AcceptClient(int client_fd);
        void    CloseConnection(struct pollfd pfd);
        void    monitortimeout( void );
        void    test( void );
};

#endif