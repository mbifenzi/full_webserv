
#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../Utils.h"
# include "../config/location.hpp"


class   HTTP_Request{

    int           _Socket;
    std::string   _Method;
    std::string   _Protocol;
	std::string   _Path;
    std::string   _Query;
	std::string   _Host;
	int			  _Port;
    std::map<std::string, std::string> _Headers;
    std::string   _Body;
    int           _Bodyfd;
    size_t        _chunkSize;
    std::string   _tmpBody;
    size_t        _BodySize;
    std::string   _Request;
    
    bool          _IsReadAll;
	bool		  _HeaderParsed;
	int			  _ErrorCode;
    bool          _ConnectionLost;
    time_t        _time;
    int           _timeout;
    bool          _CGIrunning;
    int           _CgiPid;

    public:
        HTTP_Request( void );
        HTTP_Request( int socket );
        ~HTTP_Request();

        void    parseRequest( void );
        void	parseHeader( std::string &req );
        void	parseBody( void );
        bool    IsDone( void );

        std::string     getHost( void ) const;
        int             getPort( void ) const;
        std::string     getPath( void ) const;
        std::string     getMethod( void ) const;
        std::string     getBody( void ) const;
        std::string     getHeaderValue(std::string name);
        int	            getSocket( void );
        bool            getConnection( void );
        std::string     getProtocol( void );
        std::string     getQuery( void );
        int             getfd( void );
        int             getBodySize( void );
        bool            IsHeaderParsed( void );
        std::string     getUploadDir( void );
        bool            CgiIsrunning( void );
        int             getCgiPid( void );

        void    setErrorCode(int error);
        void    UpdateCgi( void );
        void    setMethod( std::string & Method );
        void    setQuery( std::string & Query );
        void    setHeaders( void );
        void    setBody(std::string & Body);
        void    setCgiPid(int pid);
        void    setSocket( int socket );
        void    Print( void );
		void	Reset( void );
		int		CheckError( void );
        
		void	parse_body();
		int		get_error_code( void );
        bool    timeout( void );
};


#endif