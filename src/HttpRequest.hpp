#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>

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
  {}

  inline std::string getRequestTarget() { return m_requestTarget; }
  inline int getVersionMajor() { return m_httpVersionMajor; }
  inline int getVersionMinor() { return m_httpVersionMinor; }

  void parseRequestBuffer(const std::string& requestBuffer);

private:
  std::string m_requestTarget;
  int m_httpVersionMajor;
  int m_httpVersionMinor;

};

#endif //HTTPREQUEST_H