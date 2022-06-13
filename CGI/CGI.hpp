
#ifndef CGI_HPP
# define CGI_HPP

# include "../Utils.h"
# include "../HTTP_request/HTTP_request.hpp"
# include "../HTTP_response/HTTP_response.hpp"

class CGI{

    std::string _Path;
    int         fd_out;
    std::string output_path;
    bool        doNotEnter;


    public:
        CGI( void );
        CGI(std::string &Path);
        CGI(const CGI &c);
        ~CGI();

        CGI & operator=(const CGI &c);

        void    _SetEnv(std::string &file_path, HTTP_Request &req);
        void    _ExecGet(int fd, std::string &file_path);
        void    _ExecPost(int fd_out, int fd_in, std::string &file_path);
        void    _ExecCGI(std::string file_path, HTTP_Request &req, HTTP_Response &res);
        void    _GenerateResponse(HTTP_Response &res);

        void    test();
};

#endif