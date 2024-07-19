#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

int openSocket(uint16_t port);
int awaitConnection(int server_fd);
std::string receiveFromClient(int client_fd);
void sendResponse(int client_fd, HttpResponse response);

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  int server_fd = openSocket(4221);

  if (server_fd < 0)
    return 1;

  int client_fd = awaitConnection(server_fd);

  if (client_fd < 0)
  {
    close(server_fd);
    return 1;
  }

  std::string requestString = receiveFromClient(client_fd);

  if (requestString.empty())
  {
    close(client_fd);
    close(server_fd);
    return 1;
  }

  HttpRequest request (requestString);

  HttpResponse response;

  const std::string& target = request.getRequestTarget();

  // was a target given?
  if (!target.empty())
  {
    response.setStatus(200);
    response.setStatusString("OK");

    // check if starts with echo
    if (target.compare(0, 5, "echo/") == 0)
    {
      std::string body = target.substr(5);
      response.addHeader("Content-Type", "text/plain");
      response.addHeader("Content-Length", std::to_string(body.length()));

      response.setBody(body);
    }
  }

  sendResponse(client_fd, response);

  close(client_fd);
  close(server_fd);

  return 0;
}


/************************************************************************
 * Description
 *    opens a socket with the specified port
 *
 * Parameters
 *    port: the port to open the socket on
 *
 * Output
 *    the socket file descriptor
 *    -1 on failure
 ***********************************************************************/
int openSocket(uint16_t port)
{
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return -1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return -1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port " << std::to_string(port) << std::endl;
    return -1;
  }

  return server_fd;
}

/************************************************************************
 * Description
 *    awaits for and accepts a client to connect to the
 *    provided server socket
 *
 * Parameters
 *    server_fd: the file descriptor of the server socket
 *
 * Output
 *    the client file descriptor
 *    -1 on failure
 ***********************************************************************/
int awaitConnection(int server_fd)
{
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return -1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  std::cout << "Client connected\n";

  return client_fd;
}

/************************************************************************
 * Description
 *    pulls any data received from the client
 *
 * Parameters
 *    client_fd: the file descriptor of the client
 *
 * Output
 *    the received data
 ***********************************************************************/
std::string receiveFromClient(int client_fd)
{
  int bufferSize = 1000;
  std::string inBuffer (bufferSize, 0);

  int recLen = recv(client_fd, (void *)inBuffer.c_str(), bufferSize, 0);

  if (recLen == -1)
  {
    perror("recv()");
    return {};
  }
  else if (recLen)
  {
    std::cout << inBuffer << std::endl;
    return inBuffer;
  }
  else
  {
    std::cout << "Received EOF.\n";
    return {};
  }
}

/************************************************************************
 * Description
 *    Sends the response http response to the client
 *
 * Parameters
 *    client_fd: the file descriptor of the client
 *    response: the class containing all of the relevant response data
 *
 * Output
 *    None
 ***********************************************************************/
void sendResponse(int client_fd, HttpResponse response)
{
  std::string&& responseString = response.generateResponseString();
  send(client_fd, responseString.c_str(), responseString.length(), 0);
}