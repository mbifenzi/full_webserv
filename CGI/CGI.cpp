
#include "CGI.hpp"

CGI::CGI(){ _Path = ""; fd_out = -1; output_path=""; doNotEnter=false;};

CGI::CGI(std::string &Path): _Path(Path){ fd_out = -1; output_path= "";doNotEnter=false;};

CGI::~CGI(){ ;};

CGI::CGI(const CGI &c){
    *this = c;
}

CGI & CGI::operator=(const CGI &c)
{
    _Path = c._Path;
    fd_out = c.fd_out;
    output_path = c.output_path;
    doNotEnter = c.doNotEnter;
    return (*this);
}

void    CGI::_SetEnv(std::string &file_path, HTTP_Request &req)
{
    if (req.getHeaderValue("Cookie") != "")
		setenv("HTTP_COOKIE", req.getHeaderValue("Cookie").c_str(), 1);
	setenv("CONTENT_LENGTH", req.getHeaderValue("Content-Length").c_str(), 1);
	setenv("SERVER_PROTOCOL", req.getProtocol().c_str(), 1);
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("REDIRECT_STATUS","true", 1);
	if (req.getHeaderValue("Content-Type") != "")
		setenv("CONTENT_TYPE", req.getHeaderValue("Content-Type").c_str(), 1);
	else
		setenv("CONTENT_TYPE", "text/html; charset=utf-8", 1);
	setenv("SCRIPT_FILENAME", file_path.c_str(), 1);
	setenv("SERVER_PROTOCOL", req.getProtocol().c_str(), 1);
	setenv("QUERY_STRING", req.getQuery().c_str(), 1);
	setenv("PATH_INFO", req.getPath().c_str(), 1);		
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("REDIRECT_STATUS","true",1);
}

void    CGI::_GenerateResponse(HTTP_Response &res)
{
    char  buff[5000] = {0};
    std::string tmp = "";
    std::string line = "";
    int valread = 0;
    std::string status = "200 Ok";
    std::stringstream resp;
    char	*date = new char[50]();
    std::time_t t = std::time(0);

    int fd = open(res.getBody().c_str(), O_RDWR);
    if (fd < 0)
    {
        perror("open");
        exit(0);
    }
    while ((valread = read(fd, buff, sizeof(buff))) == sizeof(buff))
        tmp.append(buff, valread);
    if ((size_t)valread < sizeof(buff) && valread > 0)
        tmp.append(buff, valread);
    close(fd);
    remove(res.getBody().c_str());
    std::stringstream tmp_s(tmp);
    std::vector<std::string> vec = split_to_strings(tmp, "\r\n\r\n");
    getline(tmp_s, line);
    if (line.substr(0, 7) == "Status: ")
        status = line.substr(8);
    res.setCgiStatus(status);
    strftime(date, 50, "%a, %d %b %G %H:%M:%S", gmtime(&t));
    resp << "HTTP/1.1 " << status << "\r\n";
    resp << "Date: " << date << " GMT\r\n";
    delete [] date;
    resp << "Server: Shadow\r\n";
    resp << vec[0] << "\r\n";
    if (vec[1].length())
        resp << "Content-Length: " << vec[1].size() << "\r\n";
    res.setHeaders(resp.str());
    fd = open(res.getBody().c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
    write(fd, vec[1].c_str(), vec[1].size());
    close(fd);
    res.setBodySize(vec[1].size());
    res.setIsCig(true);
}

void    CGI::test()
{
    std::cout << _Path << std::endl;
    std::cout << output_path << std::endl;
}

void    CGI::_ExecCGI(std::string file_path, HTTP_Request &req, HTTP_Response &res)
{
    std::string Input = req.getBody();
    int pid = -1;
    int fd_in = -1;

    if (!req.CgiIsrunning() && !doNotEnter)
    {
        char buff[200] = {0};
        output_path = getcwd(buff, 200);
        output_path += "/upload/" + gen_random(8) + ".html";
        res.setBody(output_path);
        fd_out = open(output_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
        res.setBodyFd(fd_out);
        
        if (Input != "")
            fd_in = open(Input.c_str(), O_RDWR, 0777);
        if ((pid = fork()) < 0){
            close(fd_out);
            if (fd_in > 0)
                close(fd_in);
            if (req.getBody() != "")
                remove(req.getBody().c_str());
            res.setErrorCode(500);
            return ;
        }
        else if (pid == 0){
            
            _SetEnv(file_path, req);
            
            char *tmp[3];
            tmp[0] = (char*)_Path.c_str();
            tmp[1] = (char*)file_path.c_str();
            tmp[2] = NULL;

            dup2(fd_out, 1);
            dup2(fd_in, 0);
            close(fd_out);
            if (fd_in > 0)
                close(fd_in);
            int ret = execvp(tmp[0], tmp);
            std::cerr << "execve failed : " << strerror(errno) << std::endl;
            remove(res.getBody().c_str());
            exit(ret);
        }
        else{
            close(fd_out);
            if (fd_in > 0)
                close(fd_in);
            req.UpdateCgi();
            doNotEnter = true;
            req.setCgiPid(pid);
        }
    }
    else if (req.getCgiPid() != -1){
        int stat;
        int status = waitpid(req.getCgiPid(), &stat, WNOHANG);
        if (status > 0)
        {
            if (req.getBody() != "")
                remove(req.getBody().c_str());
            if (!WIFEXITED(stat))
            {
                res.setErrorCode(500);
                req.setCgiPid(-1);
                req.UpdateCgi();
                return ;
            }
            _GenerateResponse(res);
            req.UpdateCgi();
            req.setCgiPid(-1);
        }
    }
}