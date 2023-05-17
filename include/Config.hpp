#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <iostream>

// En esta clase se parsea y guardan todos los datos extraidos de un archivo de configuración
class Config
{

// Estructuras de datos de un servidor
public:
	
	typedef struct s_listen
	{
		std::string	host;
		int			port;
	}				t_listen;

	typedef struct s_error_page
	{
		std::string	path;
		int			n_error;
	}				t_error_page;

	typedef struct s_location
	{
		std::vector<int>	method;
		std::string			root;
		std::string			index;
		std::string			upload_path;
		std::string			cgi_extension;
		std::string			cgi_path;
		bool				autoindex;
		bool				upload_enable;
	}				t_location;

	typedef struct s_server
	{
		std::vector<t_error_page>	error_page;
		std::vector<t_location>		location;
		std::vector<t_listen>		listen;
		std::string					server_name;
		int							client_max_body_size;
	}								t_server;

private:
	std::vector<t_server>	server;
};

#endif