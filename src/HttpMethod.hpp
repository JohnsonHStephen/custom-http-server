#ifndef HTTPMETHOD_H
#define HTTPMETHOD_H

#include <string>

namespace HTTPMETHOD
{
  enum e_requestMethod {
    e_requestMethod_Get,
    e_requestMethod_Post,
    e_requestMethod_Max,
  };

  inline e_requestMethod parseRequestMethod(const std::string& methodString)
  {
    if (methodString.compare("GET") == 0)
      return e_requestMethod_Get;

    if (methodString.compare("POST") == 0)
      return e_requestMethod_Post;

    return e_requestMethod_Max;
  }
}

#endif //HTTPMETHOD_H