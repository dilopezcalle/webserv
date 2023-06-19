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
	fd_set				_currentSockets;
	int					_maxSocket;

public:
	Route(Config conf);
	Route(std::vector<Config> config_list);
	~Route();

	int	startListen(void);

private:
	int	selectRequest(void);
	int	redirectRequest(int socket_selected);
};

#endif