/*******************************************************************************
 * Copyright Ericsson AB 2021 - All Rights Reserved.
 *
 * No part of this software may be reproduced in any form without the written
 * permission of the copyright owner.
 *******************************************************************************/
#include "mySocketLib.hh"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>


AgentSocket::AgentSocket(int type, const char* ip, const char* port)
    : mSocketType(type), mSocketFd(-1), mIsBlocking(true), mIp(ip), mPort(port)
{
}

AgentSocket::~AgentSocket()
{
    close();
}

int
AgentSocket::getSocketType() const
{
    return mSocketType;
}

int
AgentSocket::getHandle() const
{
    return mSocketFd;
}

bool
AgentSocket::isBlocking() const
{
    return mIsBlocking;
}

void
AgentSocket::setBlocking(bool isBlocking)
{
    if (mSocketFd != -1)
    {
        int status = fcntl(mSocketFd, F_GETFL);

        if (isBlocking)
        {
            if (fcntl(mSocketFd, F_SETFL, status & ~O_NONBLOCK) == -1)
                std::cout<<"Failed to make socket blocking."<<strerror(errno)<<std::endl;
        }
        else
        {
            if (fcntl(mSocketFd, F_SETFL, status | O_NONBLOCK) == -1)
                std::cout<<"Failed to make socket nonblocking."<<strerror(errno)<<std::endl;
        }
    }

    mIsBlocking = isBlocking;
}

void
AgentSocket::setSockOpt(int optName)
{
    int flag = 1;

    flag = setsockopt(mSocketFd, SOL_SOCKET, optName, &flag, sizeof(flag));

    if (flag == -1)
        std::cout<<"setsockopt failed "<<optName<<strerror(errno)<<std::endl;
}

int
AgentSocket::createBindSocket()
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    close();

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;  // Allow IPv4 or IPv6
    hints.ai_socktype = mSocketType;
    hints.ai_flags    = AI_PASSIVE;  // For wildcard IP address
    hints.ai_protocol = 0;           // Any protocol

    if (0 != getaddrinfo(mIp.c_str(), mPort.c_str(), &hints, &result))
    {
        std::cout<<"Error on getaddrinfo()."<<strerror(errno)<<std::endl;
        return -1;
    }

    // getaddrinfo() returns a list of address structures.
    // Try each address until we successfully bind(2).
    // If socket(2) (or bind(2)) fails, we (close the socket and) try the next address. */
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        mSocketFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (mSocketFd >= 0)
        {
            setSockOpt(SO_REUSEADDR);
            if (bind(mSocketFd, rp->ai_addr, rp->ai_addrlen) == 0)
                break;
            else
            {
                std::cout<<"Error on bind()."<<strerror(errno)<<std::endl;;
                close();
            }
        }
        else
            std::cout<<"Error on Socket()."<<strerror(errno)<<std::endl;
    }

    freeaddrinfo(result);
    return mSocketFd;
}

void
AgentSocket::close()
{
    if (mSocketFd != -1)
        ::close(mSocketFd);

    mSocketFd = -1;
}

int
AgentSocket::handleData(char* buffer, int bufSize, operation event, void* addr, socklen_t* addrLen)
{
    int nBytes = 0;

    if (event == operation::EV_READ)
        switch (mSocketType)
        {
            case SOCK_DGRAM:
                nBytes = recvfrom(mSocketFd, buffer, bufSize, 0, reinterpret_cast<struct sockaddr*>(addr), addrLen);
                break;
            case SOCK_STREAM:
                nBytes = recv(mSocketFd, buffer, bufSize, MSG_NOSIGNAL);
                break;
        }

    if (event == operation::EV_WRITE)
        switch (mSocketType)
        {
            case SOCK_DGRAM:
                nBytes = sendto(mSocketFd, buffer, bufSize, 0, reinterpret_cast<struct sockaddr*>(addr), *addrLen);
                break;
            case SOCK_STREAM:
                nBytes = send(mSocketFd, buffer, bufSize, MSG_NOSIGNAL);
                break;
        }

    return nBytes;
}

int
AgentSocket::connectTo(const char* remotIp, const char* remotPort)
{
    int status = -1;
    int so_error;
    socklen_t len = sizeof(so_error);
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;  // Allow IPv4 or IPv6
    hints.ai_socktype = mSocketType;
    hints.ai_flags    = AI_PASSIVE;  // For wildcard IP address

    int s = getaddrinfo(remotIp, remotPort, &hints, &result);

    if (s != 0)
        std::cout<<"getaddrInfo failed."<<strerror(errno)<<std::endl;

    for (rp = result; rp != NULL && s == 0; rp = rp->ai_next)
    {
        status = connect(mSocketFd, rp->ai_addr, rp->ai_addrlen);
        // This error can come in case of non-block socket means TCP handshaking is
        // still in progress. In such case we would need to wait for sometime for the
        // handshaking to complete then we check SO_ERROR for any socket error.
        if (status < 0 && errno == EINPROGRESS)
        {
            fd_set fdset;
            struct timeval tv;
            FD_ZERO(&fdset);
            FD_SET(mSocketFd, &fdset);
            tv.tv_sec = 10;

            if (select(mSocketFd + 1, NULL, &fdset, NULL, &tv) == 1)
            {
                getsockopt(mSocketFd, SOL_SOCKET, SO_ERROR, &so_error, &len);

                if (so_error == 0)
                    status = 0;  // No error on socket.Connection made successfully
            }
        }
        else if (status < 0)
            std::cout<<"Error on Connect()."<<strerror(errno)<<std::endl;

        if (status == 0)
            break;
    }

    freeaddrinfo(result);
    return status;
}