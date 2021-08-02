/*******************************************************************************
 * Copyright Ericsson AB 2021 - All Rights Reserved.
 *
 * No part of this software may be reproduced in any form without the written
 * permission of the copyright owner.
 *******************************************************************************/
#ifndef AGENTSOCKET_HH
#define AGENTSOCKET_HH

#include <string>
#include <unistd.h>

/// Socket operation
enum class operation
{
   EV_READ,          /// Read from socket operation
   EV_WRITE          /// Write to Socket operation
};

/// Socket class provides opening a socket, binding it to ip:port and handling read/write on socket.
/// Its a generic  class ,it does not require mentioning socket family  IPV4 or IPV6.
class AgentSocket
{
public:
   /// @brief Socket constructor.
   ///
   /// @param type Type of the socket. SOCK_STREAM or SOCK_DATAGRAM
   /// @param ip Ip address for socket binding. If not passed, socket binds to localhost
   /// @param port Port number for socket binding. If not passed, socket binds to random port
   explicit AgentSocket(int type, const char* ip = "0", const char* port = "0");

   /// Closes the socket
   ~AgentSocket();

   /// Get type of the socket
   ///
   /// @return Type of socket. Udp or stream socket
   int getSocketType() const;

   /// Return socket file descriptor
   ///
   /// @return Socket fd. -1 if for invalid socket.
   int getHandle() const;

   /// Returns If socket is in blocking state
   ///
   /// @return If the socket is on blocking mode or not.
   bool isBlocking() const;

   /// Sets setsockopt flags on the socket
   ///
   /// @param optName FD's option
   void setSockOpt(int optName);

   /// Toggle blocking of socket fd
   ///
   /// @param bool 1-set to blocking, 0-set to non-blocking
   void setBlocking(bool isBlocking);

   /// Opens a socket and binds it to IP/port.
   ///
   /// @return Valid fd number if socket creation is successful else -1
   int createBindSocket();

   /// Close socket
   void close();

   /// Perform read, write operation.
   ///
   /// @param buffer Buffer to read/write from
   /// @param bufsize Buffer size
   /// @param event Enum to EV_READ/EV_WRITE
   /// @param addr Sock_addr struct later typecasted to sockaddr. Default value is nullptr
   /// @param addrLen Size of addr
   /// @return Returns the number of read/write bytes
   int handleData(char* buffer, int bufSize, operation event, void* addr = nullptr, socklen_t* addrLen = nullptr);

   /// Perform Connection establishment
   ///
   /// @param remotIp Remote IP address
   /// @param Remote port number
   /// @return 0 if connection is made else -1
   int connectTo(const char* remotIp, const char* remotPort);

   AgentSocket()              = delete;
   AgentSocket(const AgentSocket&) = delete;
   AgentSocket(AgentSocket&&)      = delete;
   AgentSocket& operator=(const AgentSocket&) = delete;
   AgentSocket& operator=(AgentSocket&&) = delete;

private:
   int mSocketType;   // Type of the socket (TCP or UDP)
   int mSocketFd;     // Socket descriptor
   bool mIsBlocking;  // Socket mode of the socket
   std::string mIp;   // Socket binding IP
   std::string mPort; // Socket binding Port
};

#endif