#include <string.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <fcntl.h>

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
	this->_chunked = false;
	_config.printConf();
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

bool Server::getChunked(void)
{
	return (this->_chunked);
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
	const char reuse = 1;

	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket < 0)
	{
		throw serverException("Cannot create socket");
		return (1);
	}
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		throw (std::runtime_error("SO_REUSEADDR"));
	fcntl(this->_socket, F_SETFL, O_NONBLOCK);

	std::cout << "Connecting server socket to"
	<< "\nADDRESS: " << inet_ntoa(this->_socketAddress.sin_addr)
	<< "\nPORT: " << ntohs(this->_socketAddress.sin_port) << std::endl;
	while (bind(this->_socket, (sockaddr *)&this->_socketAddress, this->_socketAddress_len) < 0){}
	std::cout << "===== Connection done! =====\n" << std::endl;
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
	size_t buffeSize = 1;
	char	buffer[buffeSize];
	std::vector<char> vecbuffer;
	std::string line = "";
	Request request;
	Request prevRequest;
	int bytesread = 1;
	// std::cout << "request: " << std::endl;
	while (bytesread > 0 && line != "\r\n")
	{
		bzero(buffer, buffeSize);
		bytesread = read(client_socket, buffer, buffeSize);
		if (bytesread < 0)
		{
			if (this->_chunked == true)
				this->_chunked = false;
			else
				throw serverException("Cannot read request");
			break;
		}
		for (int i = 0; i < bytesread; i++)
		{
			if (buffer[i] == '\n')
			{
				if (line == "\r")
				{
					Request tmp;
					if (this->_chunked == true)
					{
						if (checkBuffer(vecbuffer))
						{
							std::cout << "Chunked file uploaded successfully!\n";
							this->_chunked = false;
						}
						else
						{
							tmp = _lastRequest;
							tmp.changeChunked(vecbuffer);
						}
					}
					else
						tmp = Request(vecbuffer);
					vecbuffer.clear();
					if (tmp.getContentLength() || tmp.getTransEncoding() != "")
						tmp.setFileContent(client_socket);
					if ((int)tmp.getContentLength() <= this->_config.getClientMaxBodySize())
						request = tmp;
					else
						std::cout << "Error: Body is too big! Using an empty request" << std::endl;
					bytesread = 0;
				}
				else
				{
					vecbuffer.insert(vecbuffer.end(), line.begin(), line.end());
					vecbuffer.push_back('\n');
					line.clear();
				}
			}
			else
				line.push_back(buffer[i]);
		}
	}
	if (bytesread >= 0)
	{
		//std::cout << "request:\n" << request << std::endl;
		Response response(this->_config, request);
		if (!(_lastRequest == request) || (_lastRequest == request && request.getMethod() != "GET"))
		{
			response.generateResponse();
			_serverResponse = response._getFullResponse();
			if (this->_chunked == false)
				_lastRequest = request;
		}

		sendResponse(client_socket);

		if (request.getTransEncoding().find("chunked") != std::string::npos)
		{
			
			// _lastRequest.setFirstChunk();
			if (this->_chunked == false)
				this->_chunked = true;
		}
	}
	if (request.getConnection().find("keep-alive") == std::string::npos || bytesread < 0)
	{
		close(client_socket);
		printMessage("Closing connection");
	 	return (1);
	}
	printMessage("Keep-alive connection"); 
	return (0);
}

// Send response to the client socket
void Server::sendResponse(int client_socket)
{
	unsigned long bytesSent;

	std::cout << "llega\n" << _serverResponse << std::endl;
	bytesSent = write(client_socket, _serverResponse.c_str(), _serverResponse.size());
	std::cout << "sale" << std::endl;
	if (bytesSent != _serverResponse.size())
		printMessage("Error sending response to client");
}

int Server::checkBuffer(std::vector<char> buf)
{
	std::string str(buf.begin(), buf.end());
	std::cout << "============================ Buffer: " << str << std::endl;
    std::vector<std::string> lines;
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line))
        lines.push_back(line);
	// Chunked request. Only contains Content-length and Body
	if (lines[0].find("HTTP/1.1") == std::string::npos)
		return (0);
	// Normal HTTP 1.1 request
	return (1);
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
