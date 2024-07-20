#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

int openSocket(uint16_t port);
void checkForMessage(int server_fd);
HttpResponse generateHttpResponse(HttpRequest request);
void sendResponse(int client_fd, HttpResponse response);

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  int server_fd = openSocket(4221);

  if (server_fd < 0)
    return 1;

  checkForMessage(server_fd);

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

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return -1;
  }

  return server_fd;
}

/************************************************************************
 * Description
 *    accepts a client to connect to the provided server socket
 *
 * Parameters
 *    server_fd: the file descriptor of the server socket
 *
 * Output
 *    the client file descriptor
 *    -1 on failure
 ***********************************************************************/
int connectClient(int server_fd)
{
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  if (client_fd == -1)
  {
    perror("accept()");
    return client_fd;
  }

  std::cout << "Client connected\n";

  return client_fd;
}

/************************************************************************
 * Description
 *    Constantly polls the server socket and client sockets for
 *    incomming messages the server socket implies new connections
 *    loops until all conections are done and no msgs are received in 5
 *    seconds. Needs at least one connection before exiting
 *
 * Parameters
 *    server_fd: the file descriptor of the server socket
 *
 * Output
 *    the received data
 ***********************************************************************/
void checkForMessage(int server_fd)
{
  fd_set rfds;
  struct timeval tv;
  int max_fd = server_fd;
  std::vector<int> client_fds;

  // tracks if a connection has happened yet
  bool firstConnection = false;

  while (true)
  {
    FD_ZERO(&rfds);
    // Track all of the file descriptors for change
    FD_SET(server_fd, &rfds);
    for (auto client_fd = client_fds.begin(); client_fd != client_fds.end(); ++client_fd)
    {
      FD_SET(*client_fd, &rfds);
      if (*client_fd > max_fd)
        max_fd = *client_fd;
    }

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    // wait 5 seconds for a message from any client or possible new client
    int retVal = select(max_fd+1, &rfds, NULL, NULL, &tv);

    if (retVal == -1)
    {
      perror("select()");
    }
    else if (retVal)
    {
      // check if any client sent a message
      for (auto client_fd = client_fds.begin(); client_fd != client_fds.end(); ++client_fd)
      {
        if (!FD_ISSET(*client_fd, &rfds))
          continue;

        int bufferSize = 1000;
        std::string inBuffer (bufferSize, 0);

        // grab the client message
        int recLen = recv(*client_fd, (void *)inBuffer.c_str(), bufferSize, 0);

        if (recLen == -1)
        {
          perror("recv()");
        }
        else if (recLen)
        {
          std::cout << inBuffer << std::endl;
          HttpRequest request (inBuffer);
          sendResponse(*client_fd, generateHttpResponse(request));
        }
        else // they have closed the connection
          std::cout << "Received EOF.\n";

        close(*client_fd);
        // remove the closed connection
        client_fd = client_fds.erase(client_fd);
        // we need to check if we've ended now before trying to increment
        if (client_fd == client_fds.end())
          break;
      }

      // New client is attempting to connect
      if (FD_ISSET(server_fd, &rfds))
      {
        int newClient = connectClient(server_fd);
        if (newClient != -1)
        {
          // we have recieved a connection so now we can timeout
          firstConnection = true;
          client_fds.emplace_back(newClient);
        }
      }
    }
    else
    {
      std::cout << "Did not recieve any data in 5 seconds" << std::endl;
      if (firstConnection && client_fds.empty())
        break;
    }
  }

}

/************************************************************************
 * Description
 *    creates an http response based on the request
 *
 * Parameters
 *    request: the class containing all of the relevant request data
 *
 * Output
 *    the http response
 ***********************************************************************/
HttpResponse generateHttpResponse(HttpRequest request)
{
  HttpResponse response;

  const std::string& target = request.getRequestTarget();

  // empty targets need to respond with status 200
  if (target.empty())
  {
    response.setStatus(200);
    response.setStatusString("OK");

    return response;
  }

  // check target starts with echo
  if (target.compare(0, 5, "echo/") == 0)
  {
    response.setStatus(200);
    response.setStatusString("OK");

    std::string body = target.substr(5);
    response.addHeader("Content-Type", "text/plain");
    response.addHeader("Content-Length", std::to_string(body.length()));

    response.setBody(body);
  }

  // check for user-agent target
  if (target.compare(0, 10, "user-agent") == 0)
  {
    response.setStatus(200);
    response.setStatusString("OK");

    std::string body = request.getHeaderContent("User-Agent");

    if (body.length() > 0)
    {
      response.addHeader("Content-Type", "text/plain");
      response.addHeader("Content-Length", std::to_string(body.length()));

      response.setBody(body);
    }
  }

  return response;
}

/************************************************************************
 * Description
 *    Sends the http response to the client
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