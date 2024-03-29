#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "web_server.hpp"
#include "utils_config.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <algorithm>

// In this class all config params are checked and stored
class Config
{
	public:
		typedef struct s_error_page
		{
			std::string	path;
			int			n_error;
		}				t_error_page;

		typedef struct s_location
		{
			std::vector<int>	method;
			std::string			path;
			std::string			root;
			std::string			index;
			std::string			upload_path;
			std::string			cgi_extension;
			std::string			cgi_path;
			bool				autoindex;
			bool				upload_enable;
			std::string			redir_url;
		}				t_location;

	private:
		std::vector<t_error_page>	error_page;
		std::vector<t_location>		location;
		std::vector<std::string>	server_name;
		std::string					host;
		int							port;
		int							client_max_body_size;
		std::string					conf_body;
		bool						dirList;

	public:
		// Constructor
		Config();
		Config(std::string msg);
		Config(const Config &src);
		// Destructor
		~Config();

		// ===== Getters =====
		t_error_page	getErrorPage(int index);
		std::string		getHost(void);
		t_location		getLocation(int index);
		int				getPort(void);
		int				getClientMaxBodySize(void);
		int				getSizeLocation(void);
		size_t			getSizeServerName(void);
		std::vector<t_error_page>	getErrorPages(void);
		std::vector<t_location>		getLocations(void);
		bool			getDirList(void) const;

		Config	&operator=(const Config &src);
		std::string getBody(void) const;
		const std::map<std::string, std::string> &getEnvironment() const;
    	void setEnv(char** env);
		void exportEnv(const std::string &key, const std::string &value);
		void fillFields(const std::string &src);
		std::vector<std::string> fillWords(std::string line);
		void saveListen(const std::vector<std::string> &words);
		void saveServerName(std::vector<std::string> &words);
		void saveErrorPage(std::vector<std::string> &words);
		void saveMaxSize(std::vector<std::string> &words);
		void initLocation(Config::t_location &src);
		void saveLocation(std::vector<std::string> &words, std::size_t &locationIndex);
		void printEnv(void) const;
		void printConf(void) const;

	private:
		std::map<std::string, std::string> environment;
};

std::vector<std::string> extractServerBlocks(const std::string &filePath);

#endif