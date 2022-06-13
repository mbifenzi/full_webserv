
#include "Utils.h"

std::vector<std::string> split(std::string &line, char delim)
{
    std::size_t pos;
    std::size_t l_pos;
    std::vector<std::string> temp;
    std::string copy = line;

    l_pos = 0;
    while ((pos = copy.find_first_of(delim)) != std::string::npos)
    {
        temp.push_back(copy.substr(l_pos, pos));
        copy = copy.substr(pos + 1);
    }
    temp.push_back(copy);
    return (temp);
}

int ft_atoi(std::string str)
{
    
    int result = 0;

    for (std::string::iterator it = str.begin(); it != str.end(); it++)
    {
        result = result * 10 + *it - 48;
    }
    return (result);
}

std::vector<std::string> split_string(std::string raw_req, std::string delim)
{
    std::size_t pos;
    std::vector<std::string> vec;

    pos = raw_req.find(delim);
	if (pos != std::string::npos)
	{
		vec.push_back(raw_req.substr(0, pos));
    	vec.push_back(raw_req.substr(pos + delim.size()));
	}
    return (vec);
}

std::vector<std::string> split_to_strings(std::string raw_req, std::string delim)
{
    std::size_t pos;
    std::vector<std::string> vec;

	while ((pos = raw_req.find(delim)) != std::string::npos)
	{
		vec.push_back(raw_req.substr(0, pos));
    	raw_req = raw_req.substr(pos + delim.size());
	}
	vec.push_back(raw_req.substr(0, pos));
    return (vec);
}

int     hexa_to_decimal(std::string & hex)
{
    int len = hex.size();
 
    int base = 1;
 
    int dec_val = 0;
 
    for (int i = len - 1; i >= 0; i--) {
 
        if (hex[i] >= '0' && hex[i] <= '9') 
        {
            dec_val += (int(hex[i]) - 48) * base;
            base = base * 16;
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f') 
        {
            dec_val += (int(hex[i]) - 87) * base;
            base = base * 16;
        }
    }
    return dec_val;
}

std::string    ConstructPath(std::string first, std::string last)
{
    if (first.find_last_of('/') != first.size() - 1)
        first.append("/");
    if (last.find_first_of('/') == 0)
        last = last.substr(1);
    return first + last;
}

std::string l_trim(std::string &to_trim, char c)
{
    const char *tmp = to_trim.c_str();
    int i = 0;
    while (tmp[i++] == c)
        ;
    return (to_trim.substr(--i));
}

std::string gen_random(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}