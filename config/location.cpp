
#include "location.hpp"

location::location( void )
{
    _Root_dir = "";
    _Upload_dir = "";
    _RedirectPath = "";
    _Path = "";
    _Auto_index = false;
    _clientMaxBody = -1;
    _Index_pages =  std::vector<std::string> ();
    _Accepted_methods =  std::vector<std::string> ();
}

location::~location()
{
    //destructor 
}

bool    location::get_auto_index( void ) const { return _Auto_index;};

std::vector<std::string>    location::get_index_pages( void ) const { return _Index_pages; };

std::string    location::get_root_dir( void ) const{return _Root_dir; };

std::vector<std::string>    location::get_accepted_methods( void ) const{return _Accepted_methods; };

std::string    location::get_upload_dir( void ) const{return _Upload_dir; };

void    location::set_upload_dir(std::string &upload_dir) { _Upload_dir = upload_dir;};

std::string location::getRedirection( void ) const{return _RedirectPath;};

void    location::set_redirection(std::string &redirect_to){_RedirectPath = redirect_to;};

std::string  location::getPath( void ) const{return this->_Path;};

void    location::set_path(std::string & path){_Path = path;};

int  location::getclientMaxBody( void ) const{return _clientMaxBody;};

//CGI     location::getCgi(std::string  s){
//    return *(_Cgis.find(s));
//}

void    location::set_client_body_size(std::string Client_size){
    std::size_t pos = Client_size.find_first_not_of("0123456789");
    if (pos != std::string::npos && pos != std::string::npos - 1)
        throw "Parse error: Client max body size should not include "+ Client_size.substr(pos, pos + 1);
    _clientMaxBody = ft_atoi(Client_size);
};

int location::MethodExist(const std::string & method)
{
    std::vector<std::string>::iterator it;

    for (it = this->_Accepted_methods.begin(); it != this->_Accepted_methods.end(); it++)
    {
        if (!it->compare(method))
            return (1);
    }
    return (0);
}

void location::set_allowed_methods(std::string Method)
{
	int i;
    std::vector<std::string> vec = split(Method, ' ');
    std::array<std::string, 3> Methods = {"GET", "POST", "DELETE"};
    std::vector<std::string>::iterator it;

    for (it = vec.begin(); it != vec.end(); it++)
    {
        for (i = 0; i < 3; i++)
        {
            if (!(*it).compare(Methods[i]))
            {
                this->_Accepted_methods.push_back(*it);
                break ;
            }
        }
    }
}

void location::set_index_pages(std::string &Index_page){this->_Index_pages = split(Index_page, ' ');}

void    location::set_auto_index(std::string &Auto_index)
{
    if (!Auto_index.compare("on"))
        this->_Auto_index = true;
    else if (!Auto_index.compare("off"))
        this->_Auto_index = false;
}

void    location::set_root_dir( const std::string &Root_dir ){
    
    struct stat status;
    
    if (lstat(Root_dir.c_str(), &status))
        throw std::string("Parse error: something wrong with the Root " + Root_dir);
    if (!S_ISDIR(status.st_mode))
        throw std::string("Parse error: Root directory is not really a directory " + Root_dir);
    this->_Root_dir = Root_dir;
}

std::ostream & operator << (std::ostream & out, const location & l)
{
    out << "location={" << std::endl;
    out << "index_pages= ";
    std::vector<std::string> vec = l.get_index_pages();
    std::vector<std::string>::iterator it;
    for (it = vec.begin(); it != vec.end(); it++)
    {   
        out << *it << " " ;
    }
    out << std::endl << "root=" << l.get_root_dir() << std::endl;
    out << "auto_index=" << (l.get_auto_index() ? "on" : "off" )<< std::endl; 
    vec = l.get_accepted_methods();
    out << "allowed_methods=";
    for (it = vec.begin(); it != vec.end(); it++)
    {   
        out << *it << " " ;
    }
    out << std::endl << "client_max_body_size=" << l.getclientMaxBody() << std::endl;
    out << std::endl << "}" << std::endl;
    return (out);
}