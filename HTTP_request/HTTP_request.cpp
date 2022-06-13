
#include "HTTP_request.hpp"

HTTP_Request::HTTP_Request( int socket )
{
	_Socket = socket;
	_Protocol = "";
    _Method = "";
    _Query = "";
	_Path = "";
    _Body = "";
	_Host = "";
	_Port = 80;
    _Headers = std::map<std::string, std::string>();
	_IsReadAll = false;
	_HeaderParsed = false;
	_ErrorCode = -1;
	_chunkSize = 0;
	_Bodyfd = -1;
	_tmpBody = "";
	_BodySize = 0;
	_ConnectionLost = false;
	_time = time(NULL);
	_timeout = 5;
	_Request = "";
	_CGIrunning = false;
	_CgiPid = -1;
}

void	HTTP_Request::Reset(){
	_Protocol = "";
    _Method = "";
    _Query = "";
	_Path = "";
    _Body = "";
	_Host = "";
	_Port = 80;
	_Headers.erase(_Headers.begin(), _Headers.end());
	_IsReadAll = false;
	_HeaderParsed = false;
	_chunkSize = 0;
	_ErrorCode = -1;
	_Bodyfd = -1;
	_BodySize = 0;
	_ConnectionLost = false;
	_CGIrunning = false;
	_tmpBody = "";
	_Request = "";
	_CgiPid = -1;
}

HTTP_Request::HTTP_Request( void )
{
    // Constructor
}

HTTP_Request::~HTTP_Request( )
{
    // Destructor
}

void    HTTP_Request::setSocket( int socket )
{
	this->_Socket = socket;
}

int		HTTP_Request::get_error_code( void ){return this->_ErrorCode;}

std::string	HTTP_Request::getHost( void ) const{return (this->_Host);}

std::string    HTTP_Request::getPath( void ) const{return (this->_Path);}

int    HTTP_Request::getPort( void ) const{return (this->_Port);}

bool	HTTP_Request::IsDone(){return (this->_IsReadAll);}

std::string	 HTTP_Request::getMethod( void ) const{ return this->_Method; };

std::string     HTTP_Request::getBody( void ) const{ return this->_Body; };

int	HTTP_Request::getSocket( void ){return this->_Socket;};

bool	HTTP_Request::timeout( void ){return (difftime(time(NULL), _time) >= _timeout);};

bool	HTTP_Request::IsHeaderParsed( void ){return _HeaderParsed;};

int   HTTP_Request::getfd( void ){return _Bodyfd;};

int    HTTP_Request::getBodySize( void ){return _BodySize;};

void   HTTP_Request::UpdateCgi( void ){_CGIrunning = !_CGIrunning;};

bool   HTTP_Request::CgiIsrunning( void ){return _CGIrunning;};

std::string     HTTP_Request::getHeaderValue(std::string  name){
	if (_Headers.find(name) != _Headers.end())
		return (_Headers.find(name)->second);
	return ("");
}

std::string     HTTP_Request::getQuery( void ){return this->_Query;};

std::string     HTTP_Request::getProtocol( void ){return this->_Protocol;};

bool   HTTP_Request::getConnection( void ){return this->_ConnectionLost;};

void    HTTP_Request::setCgiPid(int pid){_CgiPid = pid;};

int    HTTP_Request::getCgiPid( void ){ return _CgiPid;};

void    HTTP_Request::setErrorCode(int e){_ErrorCode = e;};


void	HTTP_Request::parseHeader( std::string & req )
{
	std::string line;
	std::vector<std::string> splited;
	std::pair<std::map<std::string, std::string>::iterator, bool> pair;
	std::stringstream req_header;
	int     first = 0;

	req_header << req;
	while (std::getline(req_header, line) && line.compare("\r"))
	{
	    if (line[line.size() - 1] == '\r')
	        line.erase(line.size() - 1);
	    if (!first)
	    {
	        splited = split(line, ' ');
	        this->_Method = splited[0];
	        this->_Path = splited[1];
			if (this->_Path.find("?") != std::string::npos)
			{
				this->_Path = splited[1].substr(0, splited[1].find("?"));
				this->_Query = splited[1].substr(splited[1].find("?") + 1);
			}
			this->_Protocol = splited[2];
	        first = 1;
	    }
	    else
	    {
			splited = split(line, ':');
			if (!splited[0].compare("Host")) {
				this->_Host = splited[1].erase(0, 1);
				if (splited.size() == 3)
					this->_Port = ft_atoi(splited[2]);
			}
			else {
				splited[1].erase(0, 1);
	        	pair = this->_Headers.insert(std::pair<std::string, std::string>(splited[0], splited[1]));
				if (pair.second == false)
					this->_Headers[splited[0]] = splited[1];
			}
	    }
	}
}

void	HTTP_Request::parseBody()
{
	std::vector<std::string> vec;
	std::string	line;

	if (getHeaderValue("Content-Length") != "chunked")
	{
		write(_Bodyfd, _tmpBody.c_str(), _tmpBody.size());
		_BodySize += _tmpBody.size();
		_tmpBody = "";
		if ((int)_BodySize >= ft_atoi(_Headers["Content-Length"]))
			this->_IsReadAll = true;
	}
	else
	{
		if (_tmpBody.find("\r\n") != std::string::npos && !_chunkSize)
		{
			vec = split_string(_tmpBody, "\r\n");
			_chunkSize = hexa_to_decimal(vec[0]);
			if (vec[1].size() > _chunkSize + 2)
			{
				write(_Bodyfd, vec[1].c_str(), _chunkSize);
				_BodySize += _chunkSize;
				_tmpBody = vec[1].substr(_chunkSize + 2);
				_chunkSize = 0;
			}
			else
				_tmpBody = vec[1];
		}
		else if (_chunkSize + 2 <= _tmpBody.size())
		{
			write(_Bodyfd, _tmpBody.c_str(), _chunkSize);
				_BodySize += _chunkSize;
			_tmpBody = _tmpBody.substr(_chunkSize + 2);
			_chunkSize = 0;
		}
		if ((int)_BodySize >= ft_atoi(_Headers["Content-Length"]))
			this->_IsReadAll = true;
	}
	if (this->_IsReadAll)
	{
		close(_Bodyfd);
		if ((int)_BodySize != ft_atoi(_Headers["Content-Length"]))
		{
			this->_ErrorCode = 400;
			remove(_Body.c_str());
		}
	}
}

int		HTTP_Request::CheckError()
{
	if (this->_Host.empty())
		this->_ErrorCode = 400;
	if (this->_Headers.find("Content-Length") == this->_Headers.end()
		&& _Method == "POST")
		this->_ErrorCode = 400;
	else if ( _Method == "POST" && ft_atoi(_Headers["Content-Length"]) < 0)
		this->_ErrorCode = 400;
	if (this->_ErrorCode == 400)
		return (1);
	return (0);
}

void    HTTP_Request::parseRequest()
{
    char 	buff[1024] = {0};
	int		valread = 0;
    std::vector<std::string> splited_req;

	if (_IsReadAll == false)
	{
		valread = read(_Socket, buff, sizeof(buff));
		if (valread <= 0)
		{
			_ConnectionLost = true;
			if (_Bodyfd != -1)
				close(_Bodyfd);
			return ;
		}
		if (!_HeaderParsed)
		{
			_Request.append(buff, valread);
			if (_Request.find("\r\n\r\n") != std::string::npos)
			{
				splited_req = split_string(this->_Request, "\r\n\r\n");
				this->parseHeader(splited_req[0]);
				_HeaderParsed = true;
				_tmpBody = splited_req[1];
				if (this->CheckError() || _Method != "POST")
				{
					this->_IsReadAll = true;
					return ;
				}
				this->_Body = getcwd((char*)this->_Body.c_str(), 200);
				this->_Body += "/upload/" + gen_random(6);
				if (this->_Headers["Content-Type"].substr(this->_Headers["Content-Type"].find_first_of("/") + 1) != "plain")
					this->_Body +=  "." + this->_Headers["Content-Type"].substr(this->_Headers["Content-Type"].find_first_of("/") + 1);
				if ((this->_Bodyfd = open(this->_Body.c_str(),  O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0)
				{
					if (errno == ENOENT)
						_ErrorCode = 404;
					else if (errno == EACCES)
						_ErrorCode = 403;
					else
						_ErrorCode = 500;
					this->_IsReadAll = true;
					return ;
				}
				fcntl(_Bodyfd, F_SETFL, O_NONBLOCK);
				parseBody();
			}
		}
		else{
			if (_Bodyfd == -1)
			{
				this->_Body = getcwd((char*)this->_Body.c_str(), 200);
				this->_Body += "/upload/" + gen_random(6);
				if (this->_Headers["Content-Type"].substr(this->_Headers["Content-Type"].find_first_of("/") + 1) != "plain")
					this->_Body +=  "." + this->_Headers["Content-Type"].substr(this->_Headers["Content-Type"].find_first_of("/") + 1);
				if ((this->_Bodyfd = open(this->_Body.c_str(),  O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0)
				{
					if (errno == ENOENT)
						_ErrorCode = 404;
					else if (errno == EACCES)
						_ErrorCode = 403;
					else
						_ErrorCode = 500;
					this->_IsReadAll = true;
					return ;
				}
				fcntl(_Bodyfd, F_SETFL, O_NONBLOCK);
			}
			_tmpBody.append(buff, valread);
			parseBody();
		}
	}
}

void	HTTP_Request::Print( void )
{
	std::map<std::string, std::string>::iterator it;

	std::cout << this->_Method + " " + this->_Path + " " + this->_Protocol << std::endl;
	std::cout << "Host: " + this->_Host + ":" << this->_Port << std::endl;
	for (it = this->_Headers.begin(); it != this->_Headers.end(); it++)
	{
		std::cout << it->first + ": " + it->second << std::endl;
	}
	std::cout << std::endl;
	std::cout << this->_Body << std::endl;
}