#include "HttpResponse.hpp"

void HttpResponse::addHeader(const std::string &title, const std::string &content)
{
  m_headers.emplace(title, content);
}

std::string HttpResponse::generateResponseString()
{
  // Status Header
  std::string response = "HTTP/" + std::to_string(m_httpVersionMajor) + "." + std::to_string(m_httpVersionMinor);
  response += " " + std::to_string(m_status) + " " + m_statusString;
  response += "\r\n";

  // Headers
  for (auto header = m_headers.begin(); header != m_headers.end(); ++header)
    response += header->first + ": " + header->second + "\r\n";
  response += "\r\n";

  // Body
  response += m_responseBody;

  return response;
}
