#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../Utils.h"


class location {

    std::vector<std::string>    _Index_pages;
    std::string                 _Root_dir;
    bool                        _Auto_index;
    std::vector<std::string>    _Accepted_methods;
    std::string                 _Upload_dir;
    std::string                 _RedirectPath;
    std::string                 _Path;
    int                         _clientMaxBody;
    

    public:
        location( void );
        ~location();

        bool    get_auto_index( void ) const;
        std::vector<std::string>    get_index_pages( void ) const;
        std::string    get_root_dir( void ) const;
        std::vector<std::string>    get_accepted_methods( void ) const;
        std::string    get_upload_dir( void ) const;
        std::string    get_redirection( void ) const;
        int    MethodExist(const std::string & method);
        std::string getUploadDir( void ) const;
        std::string getRedirection( void ) const;
        std::string  getPath( void ) const;
        int  getclientMaxBody( void ) const;

        void    set_client_body_size(std::string s);
        void    set_path(std::string & path);
        void    set_redirection(std::string &redirect_to);
        void    set_upload_dir(std::string &upload_dir);
        void    set_auto_index( std::string &Auto_index );
        void    set_root_dir( const std::string &Root_dir );
        void    set_accepted_methods( std::vector<std::string> &Accepted_methods );

        void    set_allowed_methods(std::string Method);
        void    set_index_pages(std::string &Index_page);
};
std::ostream & operator << (std::ostream & out, const location & l);

#endif