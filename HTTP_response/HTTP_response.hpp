
#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include "../Utils.h"
# include "../HTTP_request/HTTP_request.hpp"
# include "../config/location.hpp"

class CGI;

class HTTP_Response{

	int			_Socket;
	std::string	_Headers;
	std::string	_BodyPath;
	int			_BodyFd;
	size_t		_BodySize;
	bool		_AutoIndex;
	size_t		_IndexSize;
	std::string _IndexBody;
	int			_ErrorCode;
	std::string	_Connection;
	std::string	_BodyError;
	bool		_isCgi;
	std::string	_CgiStatus;
	CGI			*_Cgi;
	std::string _Error500;

	int			_bitsRead;
	bool		_HeaderSent;
	bool 		_ResSent;
	bool		_ConnectionLost;
	std::string _status;
	std::string _date;
	std::string	_Url;
	int 		_bitsSent;

	public:
		HTTP_Response( void );
		HTTP_Response( int Socket );
		~HTTP_Response();

		bool	getAutoIndex( void );
		bool	getResDone( void);
		std::string	getConnection( void );
		int	getSocket( void );
		bool 	IsConnectionLost( void );
		int		getfd(void);
		std::string getBody(void);

		void	setErrorPages(std::string error);
		void	setCgiStatus(std::string &c);
		void	setIsCig(bool c);
		void	setBodySize(size_t b);
		void	setHeaders(std::string Headers);
		void	setBody(std::string &body_path);
		void	setBodyFd(int fd);
		void	setErrorCode(int error);
		void	build(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis);
		void	buildError(std::string ErrorCode, location &loc, std::map<int, std::string> Error_pages);
		void	buildGet(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis);
		int		builAutoIndex(std::string RootPath, std::string ReqPath);
		void	Delete(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages);
		void	Post(HTTP_Request &req, location &loc, std::map<int, std::string> Error_pages, std::map<std::string, CGI> Cgis);
		int		SendResponse( void );
		void	ExecCGI( HTTP_Request &req );
		void	CgiError( std::string error );
		bool	IsCGI( void );

		void	Logger(std::string  status);
		void	DeleteFile(HTTP_Request &req);
};

#endif
