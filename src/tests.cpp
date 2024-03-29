#include "tests.hpp"
#include <unistd.h>

void testConfigs(std::vector<Config> configs)
{
    std::vector<int> ports;
    std::vector<std::string> hosts;
    if (configs.empty())
		throw std::runtime_error("Error: invalid config file path.");
    for (size_t i = 0; i < configs.size(); i++)
    {
        if (configs[i].getHost().empty())
            throw std::runtime_error("Error: host is empty.");
        if (configs[i].getSizeServerName() == 0)
            throw std::runtime_error("Error: server_name is empty.");
        // Checks if port is in registered ports range
        if (1024 > configs[i].getPort() || configs[i].getPort() > 49151)
            throw std::runtime_error("Error: invalid port number.");
        // Check if there are duplicated ports
        if (std::find(ports.begin(), ports.end(), configs[i].getPort()) != ports.end())
            throw std::runtime_error("Error: duplicated ports.");
        ports.push_back(configs[i].getPort());
        // Minitest for error_pages
        testErrorPage(configs[i].getErrorPages());
        // Minitests for location{}
        testLocation(configs[i].getLocations());
    }
}

void testErrorPage(std::vector<Config::t_error_page> errors)
{
    size_t errorPageOk = 0;

    if (errors.empty())
        throw std::runtime_error("Error: you need at least one error page.");
    for (size_t i = 0; i < errors.size(); i++)
    {
        // At least should be one error page
        if (errors[i].n_error == 404)
            errorPageOk = 1;
        // Checks if there are duplicated paths or error numbers
        for (size_t j = 0; j < errors.size(); j++)
            if (j != i && (errors[i].n_error == errors[j].n_error || errors[i].path == errors[j].path))
                throw std::runtime_error("Error: duplicated values in error pages.");
    }
    if (errorPageOk == 0)
        throw std::runtime_error("Error: 404 error page not found.");
}

void testLocation(std::vector<Config::t_location> locations)
{
    if (locations.empty())
		throw std::runtime_error("Error: you need at least one location.");
    size_t root = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        // Checking there is at least one method
        if (locations[i].method.size() < 1)
            throw std::runtime_error("Error: Missing location method.");
        // Checking if there is more than one '/'
        if (locations[i].path == "/")
            root++;
        // Checking root
        if (locations[i].root.empty())
            throw std::runtime_error("Error: Missing location root.");
        // If autoindex=off you need an upload path
        if (locations[i].upload_enable && locations[i].upload_path.empty())
            throw std::runtime_error("Error: upload enabled but missing path.");
        if (locations[i].upload_enable == false && !locations[i].upload_path.empty())
            throw std::runtime_error("Error: upload path with upload disabled.");
        // If there is index, checks path
        if (!locations[i].index.empty())
        {
            if (locations[i].upload_enable)
                throw std::runtime_error("Error: index and upload_path.");
            if (locations[i].index != "index.html")
                checkPath(joinPaths(locations[i].root, locations[i].index));
        }
        else if (locations[i].upload_enable == false)
            throw std::runtime_error("Error: missing index");
        if(locations[i].redir_url != "")
        {
            // If there is a redir url, GET must be in location
            bool isGet = false;
            for(std::vector<int>::iterator it = locations[i].method.begin(); it != locations[i].method.end(); it++)
                if (*it == GET)
                    isGet = true;
            if (!isGet)
                throw std::runtime_error("Error: redirection url without GET in location");
            // If there is a redir url it must start by http or https
            std::string httpPrefix = "http://";
            std::string httpsPrefix = "https://";
            if (!(locations[i].redir_url.substr(0, httpPrefix.size()) == httpPrefix || \
                locations[i].redir_url.substr(0, httpsPrefix.size()) == httpsPrefix))
                throw std::runtime_error("Error: redirection url must include 'http://' or 'https://' prefix.");
        }
    }
    if (root != 1)
		throw std::runtime_error("Error: you need one and only one root '/' location.");
}

std::string joinPaths(std::string root, std::string path)
{
    std::string result = "";
    // Cheks if it is empty
    if (path.empty())
        return root;
    if (root.empty())
        return path;
    // Checks if path is absolute
    if (path[0] == '/')
        path = path.substr(1);
    // Checks if path starts with './'
    if (path.substr(0, 2) == "./")
        path = path.substr(2);
    // Checks if path includes the root path
    if (path.substr(0, root.size()) == root)
        return path;
    // Checks if it has '/'
    if (!root.empty() && root.back() != '/')
        return root + '/' + path;
    return root + path;
}

void checkPath(const std::string &path)
{
    if (access(path.c_str(), F_OK) != 0)
        throw std::runtime_error("Error: invalid path '" + path + "'.");
}
