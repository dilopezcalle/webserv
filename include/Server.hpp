#ifndef SERVER_HPP
# define SERVER_HPP

#include <arpa/inet.h>

#include "Config.hpp"
#include "Response.hpp"

class Server
{
private:
	struct sockaddr_in	_socketAddress;
	unsigned int		_socketAddress_len;
	std::string			_ip_address;
	int					_socket;
	int					_port;
	std::string			_serverResponse;
	Config				_config;
	std::vector<int>	_clientSockets;
	Request				_lastRequest;
	bool				_chunked;

public:
	// ===== Constructor =====
	Server(Config conf);

	// ===== Destructor =====
	~Server();

	// ===== Getters =====
	int	getSocket(void);
	int	getClientSocket(int index);
	int	getSizeClientSockets(void);
	bool getChunked(void);

	// ===== Setters =====
	int	pushClientSocket(int client_socket);

	// ===== Methods =====
	int	startServer(void);
	int	acceptConnection(void);
	int	handleConnection(int client_socket);
	int	deleteClientSocket(int client_socket);
	int checkBuffer(std::vector<char> buf);

	// ===== Exception =====
	class serverException : public std::exception
    {
	private:
        char *_error;

    public:
		serverException(const char *error);
		virtual const char *what() const throw();
    };

private:
	// ===== Private methods =====
	std::string buildResponse(void);
	void sendResponse(int client_socket);
};

#endif