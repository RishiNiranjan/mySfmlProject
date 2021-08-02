#include "mySocketLib.hh"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main()
{
  char data[1024];

  AgentSocket sock{SOCK_STREAM, "127.0.0.1", "8080"};
  
  sock.createBindSocket();
  sock.setSockOpt(SO_REUSEADDR);
  sock.setBlocking(true);

  sock.connectTo("127.0.0.1", "8081");

  std::cout<<sock.getHandle();

  sock.handleData(data, 1024, operation::EV_READ);

  std::cout<<data;
}