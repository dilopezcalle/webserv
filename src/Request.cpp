#include "Request.hpp"
#include <string.h>

Request::Request(){
    this->_method = "";
    this->_route = "";
    this->_protocol = "";
    this->_host = "";
    this->_connection = "";
    this->_boundary = "";
    this->_fileName = "";
    this->_transEncoding = "";
    this->_expect = "";
    this->_access_control = "";
    this->_contentLength = 0;
    this->_fileExist = false;
}

Request::Request(std::vector<char> buf)
{
    this->_method = "";
    this->_route = "";
    this->_protocol = "";
    this->_host = "";
    this->_connection = "";
    this->_boundary = "";
    this->_fileName = "";
    this->_full_request = buf;
    this->_contentLength = 0;
    this->_transEncoding = "";
    this->_expect = "";
    this->_fileExist = false;
    this->_access_control = "";
    getInfo();
}

bool Request::operator==(const Request& other) const
{
    return _full_request == other._full_request &&
           _method == other._method &&
           _route == other._route &&
           _protocol == other._protocol &&
           _host == other._host &&
           _connection == other._connection &&
           _fileName == other._fileName &&
           _fileContent == other._fileContent &&
           _transEncoding == other._transEncoding &&
           _fileExist == other._fileExist &&
           _access_control == other._access_control;
}

Request	&Request::operator=(const Request &src)
{
    if (this != &src)
    {
        this->_full_request = src._full_request;
        this->_method = src._method;
        this->_route = src._route;
        this->_protocol = src._protocol;
        this->_host = src._host;
        this->_connection = src._connection;
        this->_boundary = src._boundary;
        this->_fileName = src._fileName;
        this->_contentLength = src._contentLength;
        this->_transEncoding = src._transEncoding;
        this->_contentType = src._contentType;
        this->_contentRange = src._contentRange;
        this->_fileContent = src._fileContent;
        this->_expect = src._expect;
        this->_fileExist = src._fileExist;
        this->_access_control = src._access_control;
    }
    return (*this);
}

Request::~Request() {}

void Request::setFullRequest(const std::vector<char> &src) {
    this->_full_request = src;
}

void Request::setFileExist(bool status) {
    this->_fileExist = status;
}

std::vector<char> Request::getFullRequest(void) const {
    return this->_full_request;
}

std::string Request::getMethod(void) const {
    return this->_method;
}

std::string Request::getRoute(void) const {
    return this->_route;
}

std::string Request::getProtocol(void) const {
    return this->_protocol;
}

std::string Request::getHost(void) const {
    return this->_host;
}

std::string Request::getConnection(void) const {
    return this->_connection;
}

std::string Request::getFilename(void) const {
    return this->_fileName;
}

std::vector<char> Request::getFileContent(void) const {
    return this->_fileContent;
}

size_t Request::getContentLength(void) const{
    return this->_contentLength;
}

std::string Request::getContentType(void) const {
    return this->_contentType;
}

std::string Request::getTransEncoding(void) const {
    return this->_transEncoding;
}

std::string Request::getExpect(void) const {
    return this->_expect;
}
int Request::getFileExist(void) const {
    return (this->_fileExist);
}

std::string Request::getAccessControl(void) const {
    return (this->_access_control);
}

void Request::getInfo(void)
{
    std::string str(this->_full_request.begin(), this->_full_request.end());
    std::vector<std::string> lines;
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line))
        lines.push_back(line);
    for (std::vector<std::string>::size_type i = 0; i < lines.size(); i++)
    {
        if (i == 0)
        {
            //GET / HTTP/1.1
            std::istringstream iss(lines[0]); 
            std::string word;
            while(iss >> word)
                words.push_back(word);
            this->_method = words[0];
            this->_route = words[1];
            this->_protocol = words[2];
        }
        else
        {
            std::string line = lines[i];
            std::string::size_type pos = line.find_first_of(" \t");
            if (pos != std::string::npos) 
            {
                std::string firstWord = line.substr(0, pos);
                // Saving host
                if (firstWord == "Host:")
                    this->_host = line.substr(pos + 1);
                // Saving connection
                if (firstWord == "Connection:")
                    this->_connection = line.substr(pos + 1);
                if(firstWord == "Content-Length:")
                {
                    std::istringstream iss(line.substr(pos + 1));
                    iss >> this->_contentLength;
                }
                // Saving Transfer-Encoding
                if (firstWord == "Transfer-Encoding:" || firstWord == "Content-Range:")
                {
                    this->_transEncoding = line.substr(pos + 1);
                    this->_connection = "keep-alive";
                }
                if (firstWord == "Access-Control-Request-Method:")
                    this->_access_control = line.substr(pos + 1);
            }
            pos = line.find("boundary=");
            if (pos != std::string::npos)
                this->_boundary = line.substr(pos + 9);
            pos = line.find("Expect:");
            if (pos != std::string::npos)
            {
                std::cout << "DA VALOR A EXPECT" << std::endl;
                this->_expect = line.substr(pos + 7);
            }
            pos = line.find("filename=");
            if (pos != std::string::npos && this->_transEncoding != "")
            {
                // Saving filename in curl requests
                std::string st = line.substr(pos + 9);
                st.erase(std::remove(st.begin(), st.end(), '\"'), st.end());
                st.erase(std::remove(st.begin(), st.end(), '\r'), st.end());
                this->_fileName = st;
            }
        }
    }
}

void Request::setFileContent(int clilent_socket)
{
    int bytesToRead = 1024;
    std::vector<char> vecBody;
    int totalRead = 0;
    char buffer[bytesToRead];
    int readNum = this->_contentLength / bytesToRead;
    if (this->_contentLength % bytesToRead)
        readNum++;
    for(int i = 0; i < readNum; i++)
    {
        bzero(buffer, bytesToRead);
        int bytesRead = read(clilent_socket, buffer, bytesToRead);
        if (bytesRead > 0)
        {
            for (int i = 0; i < bytesRead; i++)
                vecBody.push_back(buffer[i]);
            totalRead += bytesRead;
        }
    }
    std::string st = "";
    std::string strBody(vecBody.begin(), vecBody.end());
    std::string strFullReq(this->_full_request.begin(), this->_full_request.end());
    std::string aux = strBody;

    // Saving filename
    std::string::size_type pos = aux.find("filename=");
    if (pos == std::string::npos)
    {
        aux = strFullReq;
        pos = aux.find("filename=");
    }
    if (pos != std::string::npos)
    {
        pos = pos + 9;
        st = aux.substr(pos, aux.find("\n", pos) - pos);
        st.erase(std::remove(st.begin(), st.end(), '\"'), st.end());
        st.erase(std::remove(st.begin(), st.end(), '\r'), st.end());
        this->_fileName = st;
    }
    aux = strBody;

    // Saving content type
    std::string::size_type pos2 = aux.find("Content-Type: ");
    if (pos2 == std::string::npos)
    {
        aux = strFullReq;
        pos2 = aux.find("Content-Type: ");
    }
    if (pos2 != std::string::npos)
    {
        pos2 = pos2 + 14;
        st = aux.substr(pos2, aux.find("\n", pos2) - pos2);
        st.erase(std::remove(st.begin(), st.end(), '\r'), st.end());
        this->_contentType = st;
    }

    // Saving filecontent
    if (this->_boundary.size() != 0)
    {
        std::string::size_type bodyStart = strBody.find("\r\n\r\n", pos) + 4;
        std::string::size_type bodyEnd = strBody.size() - this->_boundary.size() - 5;
        std::string str(vecBody.begin() + bodyEnd, vecBody.end());
        for(size_t i = 0; (bodyStart + i) < bodyEnd; i++)
            this->_fileContent.push_back(vecBody[i + bodyStart]);
    }
    else
    {
        for(size_t i = 0; i < strBody.size(); i++)
            this->_fileContent.push_back(vecBody[i]);
    }
}

void Request::changeChunked(std::vector<char> buf)
{
    this->_expect = "";
    std::string str(buf.begin(), buf.end());
    std::vector<std::string> lines;
    std::stringstream ss(str);
    std::string line;
    std::getline(ss, line);

    // Line 0: Content-Length
    std::istringstream iss(line);
    iss >> this->_contentLength;

    // Finds the position of the first newline character
    size_t newlinePos = str.find('\n');
    this->_fileContent.clear();
    if (newlinePos != std::string::npos)
        this->_fileContent.assign(buf.begin() + newlinePos + 1, buf.end());
}

void Request::setExpect(std::string str)
{
    this->_expect = str;
}

std::ostream & operator<<(std::ostream &ost, const Request &src)
{
    ost << "-> METHOD: " << src.getMethod() << std::endl \
        << "-> ROUTE: " << src.getRoute() << std::endl \
        << "-> PROTOCOL: " << src.getProtocol() << std::endl \
        << "-> CONNECTION: " << src.getConnection() << std::endl \
        << "-> HOST: " << src.getHost() << std::endl \
        << "-> TRANSFER-ENCODING: " << src.getTransEncoding() << std::endl \
        << "-> EXPECT: " << src.getExpect() << std::endl \
        << "-> CONTENT LENGHT: " << src.getContentLength() << std::endl \
        << "-> CONTENT TYPE: " << src.getContentType() << std::endl \
        << "-> ACCESS CONTROL: " << src.getAccessControl() << std::endl \
        << "-> FILENAME: " << src.getFilename() << std::endl;
    ost << "-> FILECONTENT: ";
    for (size_t i = 0; i < src._fileContent.size(); i++)
        ost << src._fileContent[i];
    return ost;
}
