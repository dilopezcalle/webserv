#include <string.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

#include "Server.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "web_server.hpp"

// ===== Constructor =====
Server::Server(Config conf)
{
	this->_config = conf;
	this->_ip_address = this->_config.getHost();
	this->_port = this->_config.getPort();
	this->_socketAddress_len = sizeof(_socketAddress);
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
}
// ===== Destructor =====
Server::~Server()
{
	close(this->_socket);
	return ;
}

// ===== Getters =====
int	Server::getSocket(void)
{
	return (this->_socket);
}
int	Server::getClientSocket(int index)
{
	if (index < (int)this->_clientSockets.size())
		return (this->_clientSockets[index]);
	return (0);
}
int	Server::getSizeClientSockets(void)
{
	return ((int)this->_clientSockets.size());
}

// ===== Setters =====
int	Server::pushClientSocket(int client_socket)
{
	this->_clientSockets.push_back(client_socket);
	return (0);
}
int	Server::deleteClientSocket(int client_socket)
{
	std::vector<int>::iterator it = std::find(this->_clientSockets.begin(), this->_clientSockets.end(), client_socket);
	if (it != this->_clientSockets.end())
	{
		std::copy(it + 1, this->_clientSockets.end(), it);
		this->_clientSockets.pop_back();
	}
	return (0);
}

// Create a socket, associate it with a address and listen for incoming connections
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
	if (listen(this->_socket, 20) < 0)
	{
		throw serverException("Socket listen failed");
		return (1);
	}
	return (0);
}

// Accept a incoming connection and save the socket
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

// Read a request and send a response
int	Server::handleConnection(int client_socket)
{
	char	buffer[this->_config.getClientMaxBodySize()];
	
	bzero(buffer, this->_config.getClientMaxBodySize());
	if (read(client_socket, buffer, this->_config.getClientMaxBodySize()) < 0)
	{
		throw serverException("Cannot read request");
		return (1);
	}

	Request request(buffer);
	Response response(this->_config, request);

	response.generateResponse();
	_serverResponse = response._getFullResponse();

	sendResponse(client_socket);
	close(client_socket);
	printMessage("Closing connection");
	return (0);
}

// Send response to the client socket
void Server::sendResponse(int client_socket)
{
	unsigned long bytesSent;

	bytesSent = write(client_socket, _serverResponse.c_str(), _serverResponse.size());
	if (bytesSent != _serverResponse.size())
		printMessage("Error sending response to client");
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
