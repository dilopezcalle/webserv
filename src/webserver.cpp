#include <iostream>

#include "Config.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "utils_config.hpp"

int	main(int argc, char **argv, char **env)
{
	if (argc > 2)
	{
		std::cout << "Error: bad args" << std::endl;
		return 0;
	}
    Config config;
	config.setEnvironment(env);
	// config.printEnvironment();
	std::vector<Config> configs;

	if (argc == 2)
	{
		std::string str(argv[1]);
		std::vector<std::string> servers = setBody(str);
		std::vector<std::string>::const_iterator it;
        for (it = servers.begin(); it != servers.end(); ++it)
        {
            const std::string &element = *it;
            std::cout << "Element: " << element << std::endl;
        }
	}
	try
	{
		Server server(config);
		server.startServer();
		server.startListen();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
		
	
	return (0);
}
