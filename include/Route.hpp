#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <vector>
# include <iostream>

# include "Server.hpp"
# include "Config.hpp"

class Route
{
private:
	std::vector<Server> _server_list;

public:
	Route(std::vector<Config> config_list);
	~Route();

	int	startListen(void);
};

#endif