#ifndef SERVER_HPP
# define SERVER_HPP

# include "../Utils.h"
# include "location.hpp"
# include "../HTTP_request/HTTP_request.hpp"
# include "../HTTP_response/HTTP_response.hpp"
# include "../CGI/CGI.hpp"

class   server {

    std::vector<std::string>    _Server_names;
    std::map<std::string, location> _Locations;
    std::map<int, std::string> _Error_pages;
    std::string _Host;
    std::string _Root_dir;
    size_t _Client_size;
    int _Port;
    std::map<std::string, CGI>  _Cgis;

    int _Socket;
    struct sockaddr_in _Address;

    public:
        server( void );
        ~server();

        int get_port( void ) const;
        size_t get_client_size( void ) const;
        std::string get_host( void ) const;
        //std::string get_error_pages( void ) const;
        std::vector<std::string> get_server_names( void ) const;
        std::map<std::string, location> get_locations( void ) const;
        int getSocket( void ) const;
        struct sockaddr_in getAddress( void ) const;
        const std::string &getRootDir( void ) const;
        std::map<int, std::string> getErrorPages( void ) const;

        void set_root_dir(std::string &Root);
        void set_address(server & serv);
        void set_port(std::string Port);
        void set_host(std::string Host);
        void set_client_size(std::string Client_size);
        void set_error_pages(std::string Error_pages);
        void set_server_names(std::string &Server_name);
		void add_error_page(std::string &error_page);
		
        void addCgi(std::string &s);
        bool findLocation(std::string to_find);
        void add_location(std::string key, const location &Location);

        void start(void);
        void handleRequest(HTTP_Request & req, HTTP_Response & res);
        std::map<std::string, CGI> &getCgis(void);
};

std::ostream & operator << (std::ostream & out, const server & s);

#endif