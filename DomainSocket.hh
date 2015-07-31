#ifndef DOMAINSOCKET_HH
#define DOMAINSOCKET_HH

#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <stdexcept>

#define DBG(fmt, args...) fprintf(stdout, "\033[33m[Debug]__[%s]__[%s]__[%d] "fmt"\n\033[0m",\
						                  __FILE__, __FUNCTION__, __LINE__, ##args)
#define DEBUG
#ifdef DEBUG
#define ERR_LOG  DBG("Have A Error On Here!")
#else
#define ERR_LOG  
#endif

class DomainSocket
{
  public:
	// Handle client disconnection
	class Disconnected:public std::exception
	{
	  public:
		Disconnected (void)
		{
		}
		 ~Disconnected (void) throw ()
		{
		}
	};

  // 3 types of DomainSocket
  public:
	typedef enum SocketType
	{
		SERVER = 0,				// The server part
		SERVER_CLIENT,			// Used by the server to handle new clients
		CLIENT					// Client that actually connect to the server
	} SocketType;

  public:
	// Server or Client socket
	DomainSocket (const std::string & path, SocketType type, bool mode);

	// Server_client socket
	DomainSocket (int fd, const struct sockaddr_un *socket, bool mode);
	~DomainSocket (void);

	// Accept a new client and return a DomainSocket of type SERVER_CLIENT
	DomainSocket *acceptClient (void); 

	// Send a data throught the socket (Only SERVER_CLIENT or CLIENT socket)
	// flags MSG_DONTWAIT
	ssize_t sendData (const void *buf, size_t len, int flags);

	// Receive a data throught the socket (Only SERVER_CLIENT or CLIENT socket)
	// flags set to  MSG_DONTWAIT for No-blocking recvMsg
	ssize_t recvData (void *buf, size_t len, int flags);

	// Close the socket
	void closeSocket (void);

	bool initCheck (void) const;

	int getSocketFd (void) const;
	
	const struct sockaddr_un *getSocket (void) const;

  private:
	bool mInit, mSocketMode;
	DomainSocket::SocketType  mSocketType;
	int mSocketFd, mAcceptMax;
	struct sockaddr_un mSocket;
};

#endif							// DOMAINSOCKET_HH
