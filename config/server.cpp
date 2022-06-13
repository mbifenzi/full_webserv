
#include "server.hpp"

server::server( void )
{
    _Socket = -1;
    _Error_pages = std::map<int, std::string>();
    _Client_size = -1;
    _Port = 80;
    _Host = "127.0.0.1";
    _Locations =  std::map<std::string, location>();
    _Server_names =  std::vector<std::string>();
    _Cgis = std::map<std::string, CGI> ();
}

server::~server()
{
    // Destructor
}

int server::get_port( void ) const{return this->_Port;}

std::string server::get_host( void ) const { return this->_Host;}

size_t server::get_client_size( void ) const{ return this->_Client_size;}

std::vector<std::string> server::get_server_names( void ) const{return this->_Server_names;}

std::map<std::string, location> server::get_locations( void ) const{return this->_Locations;}

int server::getSocket( void ) const{return this->_Socket;}

struct sockaddr_in server::getAddress( void ) const{return this->_Address;}

const std::string  &server::getRootDir( void ) const{return (this->_Root_dir);}

std::map<int, std::string> server::getErrorPages( void ) const{ return this->_Error_pages;};

void    server::set_root_dir(std::string & Root){
    
    struct stat status;
    
    if (lstat(Root.c_str(), &status))
        throw std::string("Parse error: something wrong with the Root " + Root);
    if (!S_ISDIR(status.st_mode))
        throw std::string("Parse error: Root directory is not really a directory " + Root);
    this->_Root_dir = Root;
}

void    server::set_port(std::string Port){this->_Port = ft_atoi(Port);}

void    server::set_host(std::string Host){this->_Host = Host;}

std::map<std::string, CGI> &server::getCgis(void){return _Cgis;};

void    server::addCgi(std::string &s){
    std::vector<std::string> vec;
    struct stat status;

    vec = split(s, ' ');
    if (lstat(vec[1].c_str(), &status))
        throw std::string("Parse error: something wrong with the CGI " + vec[1]);
    //if (!S_IS(status.st_mode))
    //    throw std::string("Parse error: CGI pis not a file " + vec[1]);
    if (vec[0] == ".php" || vec[0] == ".py")
        _Cgis.insert(std::pair<std::string, CGI>(vec[0], CGI(vec[1])));
    else
        throw "Parse Error: Unrecognizble token" + vec[0];
}

void    server::set_client_size(std::string Client_size)
{
    std::size_t pos = Client_size.find_first_not_of("0123456789");
    if (pos != std::string::npos && pos != std::string::npos - 1)
        throw "Parse error: Client max body size should not include "+ Client_size.substr(pos, pos + 1);
    this->_Client_size = ft_atoi(Client_size);
}

bool     server::findLocation(std::string to_find){
    if (_Locations.find(to_find) != _Locations.end())
        return (true);
    return (false);
}

void    server::add_location(std::string key, const location &Location){
    this->_Locations.insert(std::pair<std::string, location>(key, Location));
}

void    server::set_server_names(std::string &Server_name){ this->_Server_names = split(Server_name, ' ');}

void    server::set_address(server & serv)
{
    socklen_t len = sizeof(this->_Address);
    getsockname(serv.getSocket(), (struct sockaddr*) &this->_Address, &len);
}

void	server::add_error_page(std::string &line)
{
	std::vector<std::string> vec = split(line, ' ');

    struct stat status;
    
    if (vec[0] != "403" && vec[0] != "404" && vec[0] != "405"
        && vec[0] != "413" && vec[0] != "500")
        throw std::string("Parse error: problem with error code " + vec[0]);
    if (lstat(vec[1].c_str(), &status))
        throw std::string("Parse error: Error page does not exist " + vec[0]);
    if (!S_ISREG(status.st_mode))
        throw std::string("Parse error: Error page is not a file");
	_Error_pages[ft_atoi(vec[0])] = vec[1];
}

void    server::set_error_pages(std::string Error_pages)
{
    this->_Error_pages.insert(std::pair<int, std::string>(403, Error_pages + "/403.html"));
    this->_Error_pages.insert(std::pair<int, std::string>(404, Error_pages + "/404.html"));
    this->_Error_pages.insert(std::pair<int, std::string>(405, Error_pages + "/405.html"));
    this->_Error_pages.insert(std::pair<int, std::string>(413, Error_pages + "/413.html"));
    this->_Error_pages.insert(std::pair<int, std::string>(500, Error_pages + "/500.html"));
}

void    server::start()
{
    if ((this->_Socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    int i = 1;
    if (setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    fcntl(this->_Socket, F_SETFL, O_NONBLOCK);
    this->_Address.sin_family = AF_INET;
    this->_Address.sin_addr.s_addr = inet_addr(this->_Host.c_str());
    this->_Address.sin_port = htons(this->_Port);

    memset(this->_Address.sin_zero, 0, sizeof this->_Address.sin_zero);

    if (bind(this->_Socket, (struct sockaddr*) &this->_Address, sizeof(this->_Address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    if (listen(this->_Socket, 1000) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening on:" << this->_Host + ":" << this->_Port << std::endl;
}

void server::handleRequest(HTTP_Request & req, HTTP_Response & res)
{
    std::string Path = req.getPath();
    std::map<std::string, location>::iterator it;
    std::string location = "";

    size_t longest_match = 0;

    for (it = this->_Locations.begin(); it != this->_Locations.end(); it++)
    {
        if (Path.find(it->first) == 0  && it->first.size() > longest_match)
        {
            longest_match = it->first.size();
            location = it->first;
        }
    }
    if (!longest_match)
        it = this->_Locations.find("/");
    else
        it = this->_Locations.find(location);
    //req.Print();
    res.build(req, it->second, this->_Error_pages, _Cgis);
}

std::ostream & operator << (std::ostream & out, const server & s)
{
    out << std::endl << "server" << std::endl;
    out << "host=" << s.get_host() << std::endl;
    out << "port=" << s.get_port() << std::endl;
    out << "client_body_size=" << s.get_client_size() << std::endl;
    std::vector<std::string> vec = s.get_server_names();
    std::vector<std::string>::iterator i;
    out << "server_names=";
    for (i = vec.begin(); i != vec.end(); i++)
    {
        out << *i << " ";
    }
    out << std::endl;
    return (out);
}