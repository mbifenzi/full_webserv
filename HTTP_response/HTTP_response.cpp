
#include "HTTP_response.hpp"
#include "../CGI/CGI.hpp"

HTTP_Response::HTTP_Response()
{
	_Socket = -1;
	_BodyPath = "";
	_Headers = "";
	_BodyFd = -1;
	_BodySize = 0;
	_AutoIndex = false;
	_IndexSize = 0;
	_IndexBody = "";
	_ErrorCode = -1;
	_HeaderSent = false;
	_ResSent = false;
	_ConnectionLost = false;
	_Connection = "keep-alive";
	_BodyError = "";
	_isCgi = false;
	_bitsSent = 0;
	_bitsRead = 0;
	_status = "";
	_date = "";
	_Url = "";
	_CgiStatus = "";
	_Cgi = 0;
	_Error500 = "";
}

HTTP_Response::HTTP_Response(int Socket) : _Socket(Socket)
{
	_BodyPath = "";
	_Headers = "";
	_BodyFd = -1;
	_BodySize = 0;
	_AutoIndex = false;
	_IndexSize = 0;
	_IndexBody = "";
	_ErrorCode = -1;
	_HeaderSent = false;
	_ResSent = false;
	_ConnectionLost = false;
	_Connection = "keep-alive";
	_BodyError = "";
	_isCgi = false;
	_bitsSent = 0;
	_bitsRead = 0;
	_status = "";
	_date = "";
	_Url = "";
	_CgiStatus = "";
	_Cgi = 0;
	_Error500 = "";
}

HTTP_Response::~HTTP_Response( ){
	if (_Cgi)
		delete _Cgi;
}

bool	HTTP_Response::getAutoIndex( void ){ return this->_AutoIndex;};

bool	HTTP_Response::getResDone( void ){return this->_ResSent;};

std::string	HTTP_Response::getConnection( void ){return this->_Connection;};

int	HTTP_Response::getSocket( void ){return this->_Socket;};

bool HTTP_Response::IsConnectionLost(void){return _ConnectionLost;};

void	HTTP_Response::setErrorCode(int error){_ErrorCode = error;};

void	HTTP_Response::setBody(std::string &body_path){_BodyPath = body_path;};

void	HTTP_Response::setHeaders(std::string Headers){_Headers = Headers;};

void	HTTP_Response::setCgiStatus(std::string &c){_CgiStatus = c;};

int		 HTTP_Response::getfd(void){return _BodyFd;};

void	HTTP_Response::setErrorPages(std::string error){_Error500 = error;};

void	HTTP_Response::ExecCGI(HTTP_Request &req ){_Cgi->_ExecCGI("", req, *this );};

void	HTTP_Response::setIsCig(bool c){_isCgi = c;};

void	HTTP_Response::setBodySize(size_t b){_BodySize = b;};

std::string HTTP_Response::getBody(void){return _BodyPath;};

void	HTTP_Response::setBodyFd(int fd){_BodyFd = fd;};

bool	HTTP_Response::IsCGI( void ){return _isCgi;};

void	HTTP_Response::buildError(std::string error, location &loc, std::map<int, std::string> Error_pages)
{
	std::string	errorCode = error.substr(0, 3);
	std::ostringstream resp;
	struct stat status;
	std::time_t t = std::time(0);
	char	*date = new char[50]();

	_status = error;
	strftime(date, 50, "%a, %d %b %G %H:%M:%S", gmtime(&t));
	_date = date;
	resp << "HTTP/1.1 " << error << "\r\n";
	resp << "Date: " << date << " GMT\r\n";
	resp << "Server: Shadow\r\n";
	delete [] date;
	if (!errorCode.compare("405"))
	{
		resp <<("Allow: ");
		std::vector<std::string> tmp = loc.get_accepted_methods();
		if (tmp.empty())
			resp << "GET, POST, DELETE\r\n";
		else
		{
			std::vector<std::string>::iterator it;
			for (it = tmp.begin(); it != tmp.end(); it++){
				if (it + 1 == tmp.end())
					resp << (*it) << "\r\n";
				else
					resp << (*it) << ", ";
			}
		}
	}
	if (errorCode != "400" && Error_pages.find(ft_atoi(errorCode)) != Error_pages.end())
	{
		lstat(Error_pages[ft_atoi(errorCode)].c_str(), &status);
		resp << "Content-Type: text/html\r\nContent-Length: " << (_BodySize = status.st_size) << "\r\n";
		_BodyPath = Error_pages[ft_atoi(errorCode)];
	}
	else if (errorCode == "400" || errorCode == "408")
	{
		_ErrorCode = ft_atoi(errorCode);
		this->_Connection = "close";
		if (errorCode == "400")
		{
			_BodyError = "<html><body><h1>Bad request</h1></body></html>";
			resp << "Content-Type: text/html\r\nContent-Length: " << _BodyError.size() << "\r\n";
		}
		else if (errorCode == "408")
		{
			_BodyError = "<html><body><h1>Request Timeout</h1></body></html>";
			resp << "Content-Type: text/html\r\nContent-Length: " << _BodyError.size() << "\r\n";
		}
	}
	this->_Headers = resp.str();
}

int	HTTP_Response::builAutoIndex(std::string RootPath, std::string ReqPath)
{
	std::ostringstream tmp;
	std::string	DirPath = ConstructPath(RootPath, ReqPath);
	DIR *dir;
	struct dirent *ent;
	std::string name;
	struct stat status;
	std::string tmpPath;
	char	*date = new char[20]();

	tmp << "<html><head><title>Index of " << ReqPath << "</title></head><body><h1>Index of " <<  ReqPath;
	tmp << "</h1><hr>";
	if (!(dir = opendir(DirPath.c_str())))
		return (0);
	while ((ent = readdir(dir)))
	{
		name = std::string(ent->d_name);
		tmpPath = DirPath + name;
		if (lstat(tmpPath.c_str(), &status))
			return (0);
		if (S_ISDIR(status.st_mode))
			tmp << "<div><a href=\"" << name << "/\">" << name << "/</a></div>";
		else{
			tmp << std::left << "<div><a href=\"" << name << "\">" << name << "</a>";
			strftime(date, 17, "%d-%b-%y %H:%M", gmtime(&(status.st_mtimespec.tv_sec)));
			for (size_t i = 0; i <= (100 - name.size()); i++)
				tmp << "&nbsp";
			tmp << date ;
			for (int i = 0; i < 30; i++)
				tmp << "&nbsp";
			tmp << status.st_size << "</div>";
		}
	}
	closedir(dir);
	tmp << "</body></html>";
	_IndexBody = tmp.str();
	_IndexSize = _IndexBody.size();
	_AutoIndex = true;
	delete [] date;
	return (1);
}

void	HTTP_Response::buildGet(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis)
{
	std::string	fullPath = ConstructPath(loc.get_root_dir(), req.getPath());
	std::ostringstream tmp; 
	struct stat status;
	int fd;
	int foundPage = 0;

	if ((fd = open(fullPath.c_str(), O_RDONLY)) < 0)
	{
		if (errno == ENOENT)
			buildError("404 Not Found", loc, Error_pages);
		else if (errno == EACCES)
			buildError("403 Forbidden", loc, Error_pages);
		else
			buildError("500 Internal Server Error", loc, Error_pages);
	}
	else
	{
		close(fd);
		lstat(fullPath.c_str(), &status);
		if (S_ISREG(status.st_mode))
			foundPage = 1;
		else if (S_ISDIR(status.st_mode) && req.getPath().find_last_of("/") == req.getPath().size() - 1)
		{
			std::vector<std::string> index_pages = loc.get_index_pages();
			if (!index_pages.empty())
			{
				std::vector<std::string>::iterator it;
				for (it = index_pages.begin(); it != index_pages.end(); it++)
				{
					fullPath = ConstructPath(ConstructPath(loc.get_root_dir(), req.getPath()), (*it));
					if (!lstat(fullPath.c_str(), &status))
					{
						foundPage = 1;
						break ;
					}
				}
			}
			if (!foundPage && loc.get_auto_index())
			{
				if (builAutoIndex(loc.get_root_dir(), req.getPath()))
				{
					buildError("200 OK", loc, Error_pages);
					tmp << "Content-type: text/html\r\nContent-length: " << _IndexSize << "\r\n";
					this->_Headers.append(tmp.str());
				}
				else
					buildError("500 Internal Server Error", loc, Error_pages);
				return ;
			}
		}
		else if (S_ISDIR(status.st_mode))
		{
			buildError("301 Moved Permanently", loc, Error_pages);
			tmp << "location: " << req.getPath() << "/\r\n" ;
			_Headers.append(tmp.str());
			return ;
		}
		std::string ext = "";
		if (fullPath.find_last_of(".") != std::string::npos)
			ext = fullPath.substr(fullPath.find_last_of("."));

		if (foundPage && ext != ".php" && ext != ".py")
		{
			buildError("200 OK", loc, Error_pages);
			tmp << "Content-Type: " << getMimeType(fullPath);
			tmp << "\r\nContent-Length: " << (_BodySize = status.st_size) << "\r\n";
			this->_Headers.append(tmp.str());
			_BodyPath = fullPath;
		}
		else if (foundPage)
		{
			if (Cgis.find(ext) != Cgis.end())
				_Cgi = new CGI(Cgis[ext]);
			else
			{
				buildError("404 Not Found", loc, Error_pages);
				return ;
			}
			_Cgi->_ExecCGI(fullPath, req, *this);
		}
		else 
			buildError("404 Not Found", loc, Error_pages);
	}
}

void	HTTP_Response::Delete(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages)
{
	std::string	Path = ConstructPath(loc.get_root_dir(), req.getPath());
	std::ostringstream tmp;

	if (remove(Path.c_str()))
	{
		if (errno == ENOENT)
			buildError("404 Not Found", loc, Error_pages);
		else
			buildError("403 Forbidden", loc, Error_pages);
		return ;
	}
	buildError("204 No Content", loc, Error_pages);
	this->_Headers.append("\r\n");
}

void	HTTP_Response::Post(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis)
{
	std::ostringstream tmp;
	std::string filename;
	std::string	fullPath = ConstructPath(loc.get_root_dir(), req.getPath());
	struct stat status;
	std::string ext = fullPath.substr(fullPath.find_last_of("."));
	
	if (!lstat(fullPath.c_str(), &status) && S_ISREG(status.st_mode) &&
		(ext == ".php" || ext == ".py"))
	{
		if (Cgis.find(ext) != Cgis.end())
			_Cgi = new CGI(Cgis[ext]);
		else
		{
			buildError("404 Not Found", loc, Error_pages);
			remove(req.getBody().c_str());
			return ;
		}
		_Cgi->_ExecCGI(fullPath, req, *this);
	}
	else if ((loc.getPath() != req.getPath()) || req.getPath() != "/upload/")
	{
		buildError("403 Forbidden", loc, Error_pages);
		remove(req.getBody().c_str());
	}
	else if (req.getBodySize() > loc.getclientMaxBody())
	{
		buildError("413 Payload Too Large", loc, Error_pages);
		remove(req.getBody().c_str());
	}
	else{
		buildError("201 Created", loc, Error_pages);
		filename = req.getBody();
		filename = filename.substr(filename.find_last_of("/") + 1);
		_IndexBody.append("<html><body><a href='/upload/" + filename + "'>CLICK HERE</a>to see file</body></html>");
		tmp << "location: " << req.getPath() << "upload/" << filename << "\r\n";
		tmp << "Content-Type: text/html\r\nContent-Length: " <<  _IndexBody.size() << "\r\n";
		this->_Headers.append(tmp.str());
	}
	
}

void	HTTP_Response::DeleteFile(HTTP_Request &req)
{
	int	code = ft_atoi(_status.substr(0, 1));

	if ((code == 4 || code == 5) && req.getBody() != "")
		remove(req.getBody().c_str());
}

void	HTTP_Response::build(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis)
{
	std::stringstream tmp;
	tmp << req.getHost() << ":" << req.getPort() << req.getPath();
	_Url = tmp.str();
	setErrorPages(Error_pages[500]);
	if (req.get_error_code() == 400)
		buildError("400 Bad Request", loc, Error_pages);
	else if (_ErrorCode == 408)
		buildError("408 Timeout", loc, Error_pages);
	else if (_ErrorCode == 500)
		buildError("500 Internal Server Error", loc, Error_pages);
	else if (req.get_error_code() == 408)
		buildError("408 Timeout", loc, Error_pages);
	else if (!loc.MethodExist(req.getMethod()))
		buildError("405 Method Not Allowed", loc, Error_pages);
	else if (!req.getMethod().compare("GET") && !loc.getRedirection().empty())
	{
		std::ostringstream tmp;

		buildError("301 Moved Permanently", loc, Error_pages);
		tmp << "location: " << loc.getRedirection() << "\r\n";
		this->_Headers.append(tmp.str());
	}
	else if (!req.getMethod().compare("GET"))
		buildGet(req, loc, Error_pages, Cgis);
	else if (!req.getMethod().compare("DELETE"))
		Delete(req, loc, Error_pages);
	else if (!req.getMethod().compare("POST"))
		Post(req, loc, Error_pages, Cgis);
	if (req.getHeaderValue("Connection") != "" && _ErrorCode != 408 && _ErrorCode != 400 &&
		_ErrorCode != 500)
		_Connection = req.getHeaderValue("Connection");
	DeleteFile(req);
}

void	HTTP_Response::Logger( std::string status )
{
	
	char	*date = new char[50]();
	std::time_t t = std::time(0);

	if (status != "")
		_status = status;
	int	code = ft_atoi(_status.substr(0, 1));
	strftime(date, 50, "%a, %d %b %G %H:%M:%S", gmtime(&t));
	_date = date;
	if (code == 2)
		std::cout << "\033[92m";
	else if (code == 4)
		std::cout << "\033[93m";
	else if (code == 5)
		std::cout << "\033[91m";
	else if (code == 3)
		std::cout << "\033[94m";
	if (_status == "408 Timeout")
		std::cout << _status + " " + _date << std::endl;
	else
		std::cout << _status + " " + _Url + " " + _date << std::endl;
	std::cout << "\033[97m";
	delete [] date;
}

void	HTTP_Response::CgiError(std::string error)
{
	std::string	errorCode = error.substr(0, 3);
	std::ostringstream resp;
	struct stat status;
	std::time_t t = std::time(0);
	char	*date = new char[50]();

	_status = error;
	strftime(date, 50, "%a, %d %b %G %H:%M:%S", gmtime(&t));
	_date = date;
	resp << "HTTP/1.1 " << error << "\r\n";
	resp << "Date: " << date << " GMT\r\n";
	resp << "Server: Shadow\r\n";
	delete [] date;
	if (_ErrorCode == 500){
		lstat(_Error500.c_str(), &status);
		resp << "Content-Type: text/html\r\nContent-Length: " << (_BodySize = status.st_size) << "\r\n";
		_BodyPath = _Error500;
	}
	else{
		_BodyError = "<html><body><h1>Request Timeout</h1></body></html>";
		resp << "Content-Type: text/html\r\nContent-Length: " << _BodyError.size() << "\r\n";
		remove(_BodyPath.c_str());
		_BodyPath = "";
	}
	this->_Connection = "close";
	_isCgi = false;
	_Headers = resp.str();
}

int	HTTP_Response::SendResponse()
{
	size_t valread = 0;
	char buff[RECV] = {0};
	std::stringstream tmp;

	if (!_HeaderSent)
	{
		if (_ErrorCode == 408)
			CgiError("408 Timeout");
		else if (_ErrorCode == 500)
			CgiError("500 Internal Server Error");
		_Headers.append("Connection: " + _Connection + "\r\n\r\n");
		if (!_IndexBody.empty())
			_Headers.append(_IndexBody);
		else if (_ErrorCode == 400 || _ErrorCode == 408)
			_Headers.append(_BodyError);
		if ((valread = write(_Socket, _Headers.c_str(), _Headers.size())) <= 0)
			_ConnectionLost = true;
		_HeaderSent = true;
		if (_ConnectionLost || !_BodyPath.empty())
			return (0);
		else
		{
			Logger("");
			return (1);
		}
	}
	else
	{
		std::string status = "";

		_BodyFd = open(_BodyPath.c_str(), O_RDONLY);
		fcntl(_BodyFd, F_SETFL, O_NONBLOCK);
		std::string	BodyContent = "";
		lseek(_BodyFd, _bitsRead, SEEK_CUR);
		valread = read(_BodyFd, buff, sizeof(buff));
		_bitsRead += valread;
		BodyContent.append(buff, valread);
				
		if (write(_Socket, BodyContent.c_str(), valread) <= 0)
		{
			_ConnectionLost = true;
			close(_BodyFd);
			if (_isCgi)
				remove(_BodyPath.c_str());
			_BodyFd = -1;
			return (0);
		}
		_bitsSent += valread;
		if (_BodySize == (size_t)_bitsSent)
		{
			if (_isCgi)
			{
				status = _CgiStatus;
				remove(_BodyPath.c_str());
			}
			Logger(status);
			close(_BodyFd);
			_BodyFd = -1;
			return (1);
		}
		close(_BodyFd);
	}
	return (0);
}