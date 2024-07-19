#include "HttpRequest.hpp"

void HttpRequest::parseRequestBuffer(const std::string& requestBuffer)
{
  size_t beginPos, endPos;

  beginPos = requestBuffer.find("/") + 1;
  endPos = requestBuffer.find(" ", beginPos - 1); // -1 here in case just / (empty target) is provided
  m_requestTarget = requestBuffer.substr(beginPos, endPos - beginPos);

  beginPos = requestBuffer.find("HTTP/", endPos) + 5;
  endPos = requestBuffer.find(".", beginPos);
  m_httpVersionMajor = stoi(requestBuffer.substr(beginPos, endPos - beginPos));

  beginPos = endPos + 1;
  endPos = requestBuffer.find("\r\n", beginPos);
  m_httpVersionMinor = stoi(requestBuffer.substr(beginPos, endPos - beginPos));
}
