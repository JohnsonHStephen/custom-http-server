#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>

class HttpRequest
{
public:
  HttpRequest(const std::string& requestBuffer)
  {
    parseRequestBuffer(requestBuffer);
  }

  HttpRequest(const HttpRequest& other)
      : m_requestTarget ( other.m_requestTarget )
      , m_httpVersionMajor ( other.m_httpVersionMajor )
      , m_httpVersionMinor ( other.m_httpVersionMinor )
      , m_headers ( other.m_headers )
  {}

  inline std::string getRequestTarget() { return m_requestTarget; }
  inline int getVersionMajor() { return m_httpVersionMajor; }
  inline int getVersionMinor() { return m_httpVersionMinor; }

  std::string getHeaderContent(const std::string& title);

  void parseRequestBuffer(const std::string& requestBuffer);

private:
  std::string m_requestTarget;
  int m_httpVersionMajor;
  int m_httpVersionMinor;

  std::unordered_map<std::string, std::string> m_headers;

};

#endif //HTTPREQUEST_H