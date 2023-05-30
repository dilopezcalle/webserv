#include "Config.hpp"

// Constructor
Config::Config()
{
	{
		t_error_page	page1;
		t_error_page	page2;
		t_error_page	page3;
		t_location		location1;
		t_location		location2;
		
		page1.n_error = 404;
		page1.path = "www/error/404.html";
		page2.n_error = 405;
		page2.path = "www/error/405.html";
		page3.n_error = 406;
		page3.path = "www/error/406.html";

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "www";
		location1.index = "./index.html";
		
		location2.method.push_back(POST);
		location2.path = "/upload";
		location2.upload_enable = true;
		location2.upload_path = "www/uploads";

		this->port = 8080;
		this->host = "0.0.0.0";
		this->server_name.push_back("localhost");
		this->client_max_body_size = 500000;
		this->error_page.push_back(page1);
		this->error_page.push_back(page2);
		this->error_page.push_back(page3);
		this->location.push_back(location1);
		this->location.push_back(location2);
	}
}

// Destructor
Config::~Config() {}

/* void Config::setBody(std::string &server)
{
	// Rellenar campos + tests desde el vector de strings
    
} */

std::string Config::getBody(void) const
{
	return this->conf_body;
}


void Config::setEnv(char** env)
{
	// Recorrer el arreglo de punteros a char hasta encontrar el elemento nulo (final)
	for (int i = 0; env[i] != nullptr; i++)
	{
		// Separar la cadena en la forma "clave=valor"
		std::string envStr(env[i]);
		size_t equalsPos = envStr.find('=');
		if (equalsPos != std::string::npos)
		{
			std::string key = envStr.substr(0, equalsPos);
			std::string value = envStr.substr(equalsPos + 1);
			environment[key] = value;
		}
	}
}

const std::map<std::string, std::string> &Config::getEnvironment() const
{
	return environment;
}

void Config::printEnv(void) const
{
	for (std::map<std::string, std::string>::const_iterator it = this->environment.begin(); it != environment.end(); ++it)
		std::cout << it->first << "=" << it->second << std::endl;
}

void Config::exportEnv(const std::string &key, const std::string &value)
{
	environment[key] = value;
}
