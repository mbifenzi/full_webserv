
#ifndef UTILS_H
# define UTILS_H


# include <vector>
# include <string>
# include <fstream>
# include <poll.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <array>
# include <string>
# include <vector>
# include <algorithm>
# include <iostream>
# include <map>
# include <unistd.h>
# include <sstream> 
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <iomanip> 
# include <time.h>
# include <stdio.h>
# include <signal.h>
# include <netinet/in.h>

# define SEND   5000
# define RECV   5000

std::vector<std::string>    split(std::string &line, char delim);
int                         ft_atoi(std::string str);
std::vector<std::string>    split_string(std::string raw_req, std::string delim);;
std::vector<std::string>    split_to_strings(std::string raw_req, std::string delim);
std::string	                rtrim(std::string & str, std::string & to_trim);
int                         hexa_to_decimal(std::string & hex);
std::string                 ConstructPath(std::string first, std::string last);
std::string                 getMimeType(const std::string & file);
std::string                 l_trim(std::string &to_trim, char c);
std::string                 gen_random(const int len);

#endif