#include "utils_config.hpp"

std::vector<std::string> setBody(std::string &path)
{
    std::vector<std::string>	conf_servers;
    std::stringstream buffer;
    std::string conf_body;
    std::ifstream file(path);
    if (!file.good())
        throw std::runtime_error("Error: File '" + path + "' not found.");
    buffer << file.rdbuf();
    // Removing comments
    std::istringstream iss(buffer.str());
    std::string line;
    while (std::getline(iss, line)) {
        std::size_t commentPos = line.find_first_of("#;");
        if (commentPos != std::string::npos)
            line.erase(commentPos);
        conf_body += line + '\n';
    }
    // Get the server{} blocks from the body
    conf_servers = extractServerBlocks(conf_body);
    return conf_servers;
}

std::vector<std::string> extractServerBlocks(const std::string &configContent)
{
    std::vector<std::string> serverBlocks;

    std::size_t pos = 0;

    while (pos != std::string::npos) {
        std::size_t start = configContent.find("server {", pos);
        std::size_t next = configContent.find("server {", start + 9);
        if (next == std::string::npos)
            next = configContent.size();
        std::size_t end = configContent.rfind("}", next) + 1;
        if (start != std::string::npos && end != std::string::npos && start < end)
        {
            serverBlocks.push_back(configContent.substr(start, end - start));
            pos = end;
        } else {
            pos = std::string::npos;
        }
    }
    return serverBlocks;
}

bool isValidIPAddress(const std::string &ipAddress)
{
    std::string segment;
    std::istringstream iss(ipAddress);
    int numSegments = 0;

    while (std::getline(iss, segment, '.'))
    {
        if (numSegments >= 4) {
            return false;
        }
        
        int num = 0;
        std::istringstream segmentIss(segment);
        if (!(segmentIss >> num) || num < 0 || num > 255)
        {
            return false;
        }
        
        numSegments++;
    }
    return numSegments == 4;
}
