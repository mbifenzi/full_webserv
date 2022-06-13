
#include "http_server.hpp"

#define POLLRDHUP 16

http_server::http_server()
{
    _Servers =  std::vector<server> ();
    _Pfds  = std::vector<struct pollfd> ();
    _Clients = std::map<int, struct sockaddr_in> ();
    _Requests = std::map<int, HTTP_Request> ();
	_Responses = std::map<int, HTTP_Response> ();
}

http_server::~http_server( void )
{
    // Destructor
}

void	http_server::parse_location(std::ifstream &config, server &Server)
{
	location tmp;
	std::string	line;
	std::vector<std::string> vec;
    std::string key;

	while (std::getline(config, line) && line.compare("  }"))
    {
        line = l_trim(line , 32);
        vec = split(line, '=');
        if (!vec[0].compare("path"))
        {
            key = vec[1];
            tmp.set_path(vec[1]);
        }
		else if (!vec[0].compare("index_pages"))
            tmp.set_index_pages(vec[1]);
        else if (!vec[0].compare("root"))
            tmp.set_root_dir(vec[1]);
        else if (!vec[0].compare("auto_index"))
            tmp.set_auto_index(vec[1]);
        else if (!vec[0].compare("allowed_methods"))
            tmp.set_allowed_methods(vec[1]);
        else if (!vec[0].compare("redirect"))
            tmp.set_redirection(vec[1]);
        else if (!vec[0].compare("client_body_size"))
            tmp.set_client_body_size(vec[1]);
        else if (!line.empty())
            throw  "Parse error: Unrecognized token : " + line;
    }
    if (tmp.getPath() == "")
        throw std::string("Parse error: no path defined in location");
    if (tmp.get_root_dir().empty())
        tmp.set_root_dir(Server.getRootDir());
    if (tmp.get_accepted_methods().empty())
        tmp.set_allowed_methods("GET POST DELETE");   
    if (tmp.getclientMaxBody() < 0 && Server.get_client_size() > 0)
        tmp.set_client_body_size(std::to_string(Server.get_client_size()));
    Server.add_location(key, tmp);
}

server    http_server::parse_server(std::ifstream &config)
{
    server tmp;
    std::string line;
    std::vector<std::string> vec;

	tmp.set_error_pages("./Error_pages");

    while (std::getline(config, line) && line.compare("}"))
    {
        line = l_trim(line , 32);
        vec = split(line, '=');
        if (!vec[0].compare("host"))
            tmp.set_host(vec[1]);
        else if (!vec[0].compare("root"))
            tmp.set_root_dir(vec[1]);
        else if (!vec[0].compare("port"))
            tmp.set_port(vec[1]);
		else if (!vec[0].compare("error_page"))
			tmp.add_error_page(vec[1]);
        else if (!vec[0].compare("client_body_size"))
            tmp.set_client_size(vec[1]);
        else if (!vec[0].compare("server_name"))
            tmp.set_server_names(vec[1]);
        else if (!vec[0].compare("location"))
            this->parse_location(config, tmp);
        else if (!vec[0].compare("CGI"))
            tmp.addCgi(vec[1]);
        else if (!line.empty() && line.compare("{"))
            throw  "Parse error: Unrecognized token : " + line;
    }
    return (tmp);
}

void    http_server::parse(std::ifstream &config)
{
    std::string line;
    std::vector<server>::iterator it;

    while (std::getline(config, line))
    {
        line = l_trim(line , 32);
        if (!line.compare("server"))
        {
            this->_Servers.push_back(this->parse_server(config));
            server tmp = _Servers.back();
            if (tmp.getRootDir() == "")
                throw std::string("Parse error: No root directory defined in server");
            if (!tmp.findLocation("/"))
            {
                location l_tmp;
                l_tmp.set_root_dir(tmp.getRootDir());
                tmp.add_location("/", l_tmp);
            }
        }
        else if (!line.empty())
            throw  "Parse error: Unrecognized token : " + line;
    }
    if (_Servers.size() == 0)
        throw std::string("Parse error: No servers defined");
    config.close();
}

void    http_server::addNewFd(struct pollfd pfd)
{
    this->_Pfds.push_back(pfd);
}

bool    http_server::PfdExist(struct pollfd pfd)
{
    std::vector<struct pollfd>::iterator it;

    for (it = this->_Pfds.begin(); it != this->_Pfds.end(); it++)
    {
        if (it->fd == pfd.fd)
            return (true);
    }
    return (false);
}

void    http_server::DeleteFd(struct pollfd pfd)
{
    std::vector<struct pollfd>::iterator it;

    for (it = this->_Pfds.begin(); it != this->_Pfds.end(); it++)
    {
        if (it->fd == pfd.fd)
            break ;
    }
    if (it != this->_Pfds.end())
        this->_Pfds.erase(it);
}

struct pollfd * http_server::pollFdTable()
{
    struct pollfd * p_fds = new struct pollfd[this->_Pfds.size()];

    for(int i = 0; i < (int)this->_Pfds.size(); i++)
    {
        p_fds[i].fd = this->_Pfds[i].fd;
        p_fds[i].events = this->_Pfds[i].events;
    }
    return (p_fds);
}

void    http_server::UpdateFd(struct pollfd pfd, int i)
{
    std::vector<struct pollfd>::iterator it;

    for (it = this->_Pfds.begin(); it != this->_Pfds.end(); it++)
    {
        if (it->fd == pfd.fd && i)
        {
            it->events = POLLOUT;
            break ;
        }
        else if (it->fd == pfd.fd && !i)
        {
            it->events = POLLIN;
            break ;
        }
    }
}

bool    http_server::isServer( int socket )
{
    std::vector<server>::iterator it;

    for (it = this->_Servers.begin(); it != this->_Servers.end(); it++)
    {
        if (it->getSocket() == socket)
            return (true);
    }
    return (false);
}

void    http_server::DirectRequest(HTTP_Request & req, struct sockaddr_in client, HTTP_Response & res)
{
    std::vector<server>::iterator it;
    std::vector<server> tmp;
    std::vector<std::string> serverNames_tmp;
    std::vector<std::string>::iterator i;

    for (it = this->_Servers.begin(); it != this->_Servers.end(); it++)
    {
        // Same IP & PORT
        if (it->getAddress().sin_addr.s_addr == client.sin_addr.s_addr &&
        it->get_port() == req.getPort())
        {
            tmp.push_back(*it);
        } 
    }
    for (it = tmp.begin(); it != tmp.end(); it++)
    {
        //Same server name
        serverNames_tmp = it->get_server_names();
        for (i = serverNames_tmp.begin(); i != serverNames_tmp.end(); i++)
        {
            if (!i->compare(req.getHost()))
                break ;
        }
        if (i != serverNames_tmp.end())
            break ;
    }
    if (it != tmp.end())
        it->handleRequest(req, res);
    else if (tmp.empty())
        this->_Servers[0].handleRequest(req, res);
    else
        tmp[0].handleRequest(req, res);
}

bool    http_server::AlreadyBind(server & _server)
{
    std::vector<server>::iterator it;

    for (it = this->_Servers.begin(); it != this->_Servers.end(); it++)
    {
        if (it->get_host() == _server.get_host() && 
            it->get_port() == _server.get_port() && it->getSocket() > 0)
        {
            _server.set_address(*it);
            return (true);
        }  
    }
    return (false);
}    

void    http_server::monitortimeout( void )
{
    std::vector<struct pollfd>::iterator it;

    for (it = _Pfds.begin(); it != _Pfds.end(); it++)
    {
        if ((_Requests.find(it->fd) != _Requests.end() &&
            _Responses.find(it->fd) == _Responses.end() &&
            _Requests[it->fd].timeout()))
        {
            HTTP_Response res(it->fd);
            if (_Requests[it->fd].getfd() != -1)
            {
                close(_Requests[it->fd].getfd());
                remove(_Requests[it->fd].getBody().c_str());
            }
            res.setErrorCode(408);
			this->_Responses.insert(std::pair<int, HTTP_Response>(it->fd, res));
            this->DirectRequest(this->_Requests[it->fd], this->_Clients[it->fd], this->_Responses[it->fd]);
            UpdateFd(*it, 1);
        }
        else if ((_Requests.find(it->fd) != _Requests.end() &&
            _Requests[it->fd].CgiIsrunning() && _Requests[it->fd].timeout()) &&
            _Requests[it->fd].getCgiPid() != -1)
        {
            if (kill(_Requests[it->fd].getCgiPid(), SIGKILL) == 0)
                _Responses[it->fd].setErrorCode(408);
            else
                _Responses[it->fd].setErrorCode(500);
            _Requests[it->fd].UpdateCgi();
            _Requests[it->fd].setCgiPid(-1);
        }
    }
}

void    http_server::CloseConnection(struct pollfd pfd)
{
    close(pfd.fd);
    this->_Clients.erase(this->_Clients.find(pfd.fd));
    this->DeleteFd(pfd);
    this->_Requests.erase(this->_Requests.find(pfd.fd));
    if (this->_Responses.find(pfd.fd) != _Responses.end())
    {
        if (_Responses[pfd.fd].getBody().size() &&
            _Responses[pfd.fd].IsCGI())
                remove(_Responses[pfd.fd].getBody().c_str());
        this->_Responses.erase(this->_Responses.find(pfd.fd));
    }
        
}

void    http_server::AcceptClient(int client_fd)
{
    struct sockaddr_in _Client;
    socklen_t client_len = sizeof(_Client);
    struct pollfd pfd;

    pfd.fd = accept(client_fd, (struct sockaddr *)&_Client, &client_len);
    if (pfd.fd == - 1)
    {
        perror("accept");
        exit(0);
    }
    else
    {
        fcntl(pfd.fd, F_SETFL, O_NONBLOCK);
        pfd.events = POLLIN;
        this->addNewFd(pfd);
        this->_Clients.insert(std::pair<int, struct sockaddr_in>(pfd.fd, _Client));
        HTTP_Request req(pfd.fd);
        this->_Requests.insert(std::pair<int, HTTP_Request>(pfd.fd, req));
    }
}

void    http_server::test()
{
    std::vector<struct pollfd>::iterator it;

    for (it = _Pfds.begin(); it != _Pfds.end(); it++)
        std::cout << it->fd << " ";
    std::cout << std::endl; 
}

void    http_server::startServers()
{
    std::vector<server>::iterator it;
    int poll_count = 0;

    for (it = this->_Servers.begin(); it != this->_Servers.end(); it++)
    {
        if (this->AlreadyBind(*it))
            continue ;
        it->start();
        struct pollfd pfd;
        pfd.fd = it->getSocket();
        pfd.events = POLLIN;
        this->addNewFd(pfd);
    }
    while (1)
    {
        struct pollfd * p_fds = this->pollFdTable();
        int    pfds_size = this->_Pfds.size();

        monitortimeout();
        poll_count = poll(p_fds, this->_Pfds.size(), 2);
        if (poll_count == -1) 
        {
            perror("poll");
            exit(1); 
        }
        if (poll_count > 0)
        {
            for (int i = 0; i < pfds_size; i++)
            {
                if (p_fds[i].revents & POLLIN)
                {
                    if (this->isServer(p_fds[i].fd))
                    {
                        AcceptClient(p_fds[i].fd);
                        continue;
                    } 
                    else
                    {
                        this->_Requests[p_fds[i].fd].parseRequest();
                        if (this->_Requests[p_fds[i].fd].getConnection())
                            CloseConnection(p_fds[i]);
                        else if (this->_Requests[p_fds[i].fd].IsDone())
                        {
						    HTTP_Response res(p_fds[i].fd);
						    this->_Responses.insert(std::pair<int, HTTP_Response>(p_fds[i].fd, res));
                            this->DirectRequest(this->_Requests[p_fds[i].fd], this->_Clients[p_fds[i].fd], this->_Responses[p_fds[i].fd]);
                            UpdateFd(p_fds[i], 1);
                        }
                        continue ;
                    }
                }
                else if ((p_fds[i].revents & POLLOUT))
                {
                    if (!_Requests[p_fds[i].fd].CgiIsrunning() && 
                        this->_Responses[p_fds[i].fd].SendResponse())
                    {
                        if (_Responses[p_fds[i].fd].getConnection() == "close")
                        {
                            close(p_fds[i].fd);
                            this->_Clients.erase(this->_Clients.find(p_fds[i].fd));
                            this->_Requests.erase(this->_Requests.find(p_fds[i].fd));
                            this->DeleteFd(p_fds[i]);
                        }
                        else
                        {
                            UpdateFd(p_fds[i], 0);
                            this->_Requests[p_fds[i].fd].Reset();
                        }
                        this->_Responses.erase(this->_Responses.find(p_fds[i].fd));
                    }
                    else if (this->_Responses[p_fds[i].fd].IsConnectionLost())
                        CloseConnection(p_fds[i]);
                    else if (_Requests[p_fds[i].fd].getCgiPid() != -1)
                        _Responses[p_fds[i].fd].ExecCGI(_Requests[p_fds[i].fd]);
                    continue ;
                }
                else if (p_fds[i].revents == POLLRDHUP)
                {
                    CloseConnection(p_fds[i]);
                    continue ;
                }
            }
        }
        delete [] p_fds;
    }
}