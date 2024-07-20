#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>

#include "HttpMethod.hpp"

class HttpRequest
{
public:
  HttpRequest(const std::string& requestBuffer)
  {
    parseRequestBuffer(requestBuffer);
  }

  HttpRequest(const HttpRequest& other)
      : m_requestMethod ( other.m_requestMethod )
      , m_requestTarget ( other.m_requestTarget )
      , m_httpVersionMajor ( other.m_httpVersionMajor )
      , m_httpVersionMinor ( other.m_httpVersionMinor )
      , m_headers ( other.m_headers )
      , m_bodyLength ( other.m_bodyLength )
      , m_requestBody ( other.m_requestBody )
  {}

  inline HTTPMETHOD::e_requestMethod getRequestMethod() { return m_requestMethod; }
  inline std::string getRequestTarget() { return m_requestTarget; }
  inline int getVersionMajor() { return m_httpVersionMajor; }
  inline int getVersionMinor() { return m_httpVersionMinor; }

  inline int getBodyLength() { return m_bodyLength; }
  inline std::string getBody() { return m_requestBody; }

  std::string getHeaderContent(const std::string& title);

  void parseRequestBuffer(const std::string& requestBuffer);

private:
  HTTPMETHOD::e_requestMethod m_requestMethod = HTTPMETHOD::e_requestMethod_Max;
  std::string m_requestTarget = {};
  int m_httpVersionMajor = 1;
  int m_httpVersionMinor = 1;

  std::unordered_map<std::string, std::string> m_headers;

  int m_bodyLength = 0;
  std::string m_requestBody = {};

};

#endif //HTTPREQUEST_H