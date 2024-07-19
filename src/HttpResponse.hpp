#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <string>

class HttpResponse
{
public:
  HttpResponse()
      : m_httpVersionMajor ( 1 )
      , m_httpVersionMinor ( 1 )
      , m_status ( 404 )
      , m_statusString ( "Not Found" )
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
  inline void setBody(const std::string& other) { m_responseBody = other; }

  void addHeader(const std::string& title, const std::string& content);

  std::string generateResponseString();

private:
  int m_httpVersionMajor;
  int m_httpVersionMinor;

  int m_status;
  std::string m_statusString;

  std::map<std::string, std::string> m_headers;

  std::string m_responseBody;

};

#endif //HTTPRESPONSE_H