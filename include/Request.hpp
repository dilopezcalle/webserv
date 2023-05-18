#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <map>

# include "web_server.hpp"

class Request
{
private:
	int			_socket;
	int			_method;
	std::string	_route;
	std::string	_protocol;

	std::map<std::string, std::string>	header;

	std::string	_body;
};

#endif