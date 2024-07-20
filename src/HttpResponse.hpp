#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <string>
#include <unordered_map>

class HttpResponse
{
public:
  HttpResponse()
  {}

  HttpResponse(const HttpResponse& other)
      : m_httpVersionMajor ( other.m_httpVersionMajor )
      , m_httpVersionMinor ( other.m_httpVersionMinor )
      , m_status ( other.m_status )
      , m_statusString ( other.m_statusString )
      , m_headers ( other.m_headers )
      , m_responseBody ( other.m_responseBody )
  {}

  inline int  getVersionMajor() { return m_httpVersionMajor; }
  inline void setVersionMajor(int other) { m_httpVersionMajor = other; }

  inline int  getVersionMinor() { return m_httpVersionMinor; }
  inline void setVersionMinor(int other) { m_httpVersionMinor = other; }

  inline int  getStatus() { return m_status; }
  inline void setStatus(int other) { m_status = other; }

  inline std::string getStatusString() { return m_statusString; }
  inline void setStatusString(const std::string& other) { m_statusString = other; }

  inline std::string getBody() { return m_responseBody; }
  void setBody(const std::string &body, const std::string &contentType = "text/plain");

  void addHeader(const std::string& title, const std::string& content);

  std::string generateResponseString();

private:
  int m_httpVersionMajor = 1;
  int m_httpVersionMinor = 1;

  int m_status = 404;
  std::string m_statusString = "Not Found";

  std::unordered_map<std::string, std::string> m_headers;

  std::string m_responseBody = {};

};

#endif //HTTPRESPONSE_H