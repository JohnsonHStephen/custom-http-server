#include "HttpRequest.hpp"

#include <iostream>

std::string HttpRequest::getHeaderContent(const std::string &title)
{
  auto header = m_headers.find(title);

  if (header != m_headers.end())
    return header->second;

  return {};
}

void HttpRequest::parseRequestBuffer(const std::string &requestBuffer)
{
  size_t beginPos, endPos;

  beginPos = 0;
  endPos = requestBuffer.find(" ", beginPos);
  m_requestMethod = HTTPMETHOD::parseRequestMethod(requestBuffer.substr(beginPos, endPos - beginPos));

  beginPos = requestBuffer.find("/", endPos) + 1;
  endPos = requestBuffer.find(" ", beginPos - 1); // -1 here in case just / (empty target) is provided
  m_requestTarget = requestBuffer.substr(beginPos, endPos - beginPos);

  beginPos = requestBuffer.find("HTTP/", endPos) + 5;
  endPos = requestBuffer.find(".", beginPos);
  try
  {
    m_httpVersionMajor = std::stol(requestBuffer.substr(beginPos, endPos - beginPos));
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << "Invalid HTTP major version: " << e.what() << std::endl;
  }
  catch (const std::out_of_range& e)
  {
    std::cerr << "HTTP major version out of range: " << e.what() << std::endl;
  }

  beginPos = endPos + 1;
  endPos = requestBuffer.find("\r\n", beginPos);
  try
  {
    m_httpVersionMinor = std::stol(requestBuffer.substr(beginPos, endPos - beginPos));
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << "Invalid HTTP minor version: " << e.what() << std::endl;
  }
  catch (const std::out_of_range& e)
  {
    std::cerr << "HTTP minor version out of range: " << e.what() << std::endl;
  }

  // go through each header
  while (endPos < requestBuffer.find("\r\n\r\n", beginPos))
  {
    beginPos = endPos + 2;
    endPos = requestBuffer.find(": ", beginPos);
    std::string headerTitle = requestBuffer.substr(beginPos, endPos - beginPos);

    beginPos = endPos + 2;
    endPos = requestBuffer.find("\r\n", beginPos);
    std::string headerContent = requestBuffer.substr(beginPos, endPos - beginPos);
    m_headers.emplace(headerTitle, headerContent);

    if (headerTitle.compare("Content-Length") == 0)
    {
      try
      {
        m_bodyLength = std::stol(headerContent);
      }
      catch (const std::invalid_argument& e)
      {
        std::cerr << "Invalid HTTP content length: " << e.what() << std::endl;
      }
      catch (const std::out_of_range& e)
      {
        std::cerr << "Content length out of range: " << e.what() << std::endl;
      }
    }
  }

  // bypass both CLRF
  beginPos = endPos + 4;
  if (beginPos + m_bodyLength <= requestBuffer.length())
  {
    m_requestBody = requestBuffer.substr(beginPos, m_bodyLength);
  }
  else
    std::cerr << "Content length is greater than body" << std::endl;
}
