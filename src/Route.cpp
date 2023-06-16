#include "Route.hpp"

#include <string.h>

Route::Route(std::vector<Config> config_list)
{
	for (int i = 0; i < (int)config_list.size(); i++)
		this->_server_list.push_back(Server(config_list[i]));
	for (int i = 0; i < (int)this->_server_list.size(); i++)
		this->_server_list[i].startServer();
}
Route::~Route(){}

int	Route::startListen(void)
{
	fd_set	current_sockets, ready_sockets;
	int		max_socket, j;

	// Initialize current set
	FD_ZERO(&current_sockets);
	for (j = 0; j < (int)this->_server_list.size(); j++)
		FD_SET(this->_server_list[j]._socket, &current_sockets);

	max_socket = this->_server_list[j - 1]._socket;

	while (1)
	{
		// Select is destructive
		ready_sockets = current_sockets;
		if (select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			throw Server::serverException(strerror(errno));
			return (1);
		}
		int i;
		for (i = 0; i <= max_socket; i++)
		{
			if (!FD_ISSET(i, &ready_sockets))
				continue ;
			for (j = 0; j < (int)this->_server_list.size(); j++)
			{
				if (i == this->_server_list[j]._socket)
				{
					// New connection
					int new_socket = this->_server_list[j].acceptConnection();
					FD_SET(new_socket, &current_sockets);
					if (new_socket > max_socket)
						max_socket = new_socket;
					this->_server_list[j]._clientSockets.push_back(new_socket);
					break ;
				}
				else
				{
					int k;
					for (k = 0; k < (int)this->_server_list[j]._clientSockets.size(); k++)
					{
						if (i != this->_server_list[j]._clientSockets[k])
							continue ;
						this->_server_list[j].handleConnection(i);
						this->_server_list[j].deleteClientSocket(i);
						FD_CLR(i, &current_sockets);
					}
					if (k < (int)this->_server_list[j]._clientSockets.size())
						break ;
				}
			}
		}
	}
	return (0);
}