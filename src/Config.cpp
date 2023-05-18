#include "Config.hpp"

// Constructor
Config::Config()
{
	{
		t_server		server;
		t_error_page	page1;
		t_error_page	page2;
		t_error_page	page3;
		t_location		location1;
		t_location		location2;
		
		page1.n_error = 404;
		page1.path = "./404.html";
		page2.n_error = 405;
		page2.path = "./405.html";
		page3.n_error = 406;
		page3.path = "./406.html";

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "/www";
		location1.index = "./homepage.html";
		
		location2.method.push_back(POST);
		location2.path = "/upload";
		location2.upload_enable = true;
		location2.upload_path = "/tmp/uploads/";

		server.port = 8080;
		server.host = "localhost";
		server.server_name.push_back("localhost");
		server.client_max_body_size = 500000;
		server.error_page.push_back(page1);
		server.error_page.push_back(page2);
		server.error_page.push_back(page3);
		server.location.push_back(location1);
		server.location.push_back(location2);

		this->_server.push_back(server);
	}

	{
		t_server		server;
		t_location		location1;

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "/home/42/website";
		location1.autoindex = false;
		location1.cgi_extension = "php";
		location1.cgi_path = "localhost:9000";

		server.port = 5000;
		server.host = "localhost";
		server.server_name.push_back("localhost");
		server.server_name.push_back("127.0.0.1");
		server.location.push_back(location1);

		// Puede que si no te pasan un dato, sea necesario poner uno por defecto
		this->_server.push_back(server);
	}}

// Destructor
Config::~Config()
{

}