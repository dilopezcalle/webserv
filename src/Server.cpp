#include <string.h>
#include <unistd.h>

#include "Server.hpp"
#include "utils.hpp"

#define BUFFER_SIZE 1024

// ===== Constructor =====
Server::Server(Config config)
{
	this->_ip_address = config.host;
	this->_port = config.port;
	this->_socketAddress_len = sizeof(_socketAddress);
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
}

// ===== Destructor =====
Server::~Server()
{
	close(this->_socket); // PUEDE GENERAR ERRORES?
}

// ===== Methods =====
int		Server::startServer(void)
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket < 0)
	{
		throw serverException("Cannot create socket");
		return (1);
	}

	if (bind(this->_socket, (sockaddr *)&this->_socketAddress, this->_socketAddress_len) < 0)
	{
		throw serverException("Cannot connect socket to address");
		return (1);
	}
	return (0);
}

int	Server::startListen(void)
{
	if (listen(this->_socket, 20) < 0)
	{
		throw serverException("Socket listen failed");
		return (1);
	}

	printMessage("===== Listening =====");
	printMessage("ADDRESS: " + std::string(inet_ntoa(this->_socketAddress.sin_addr)));
	std::cout << "PORT: " << ntohs(this->_socketAddress.sin_port) << std::endl;

	fd_set	current_sockets, ready_sockets;
	int		max_socket;

	// Initialize current set
	FD_ZERO(&current_sockets);
	FD_SET(this->_socket, &current_sockets);

	max_socket = this->_socket;

	while (1)
	{
		// Select is destructive
		ready_sockets = current_sockets;

		if (select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			throw serverException(strerror(errno));
			return (1);
		}
		for (int i = 0; i <= max_socket; i++)
		{
			if (!FD_ISSET(i, &ready_sockets))
				continue ;
			if (i == this->_socket)
			{
				// New connection
				int new_socket = acceptConnection();
				FD_SET(new_socket, &current_sockets);
				if (new_socket > max_socket)
					max_socket = new_socket;
			}
			else
			{
				handleConnection(i);
				FD_CLR(i, &current_sockets);
			}
		}
	}
	return (0);
}

int	Server::acceptConnection(void)
{
	int	new_socket;

	new_socket = accept(this->_socket, (sockaddr *)&this->_socketAddress, &this->_socketAddress_len);
	if (new_socket < 0)
	{
		throw serverException("Server failed to accept connection");
		return (1);
	}
	return (new_socket);
}

int	Server::handleConnection(int client_socket)
{
	char	buffer[BUFFER_SIZE];
	
	bzero(buffer, BUFFER_SIZE);
	if (read(client_socket, buffer, BUFFER_SIZE) < 0)
	{
		throw serverException("Cannot read request");
		return (1);
	}

	std::cout << "REQUEST: \n" << buffer << std::endl;

	close(client_socket);
	printMessage("Closing connection");
	return (0);
}

// ===== Exception =====
Server::serverException::serverException(const char *error)
{
	this->_error = (char *)error;
	return ;
};
const char *Server::serverException::what() const throw()
{
	return (this->_error);
};