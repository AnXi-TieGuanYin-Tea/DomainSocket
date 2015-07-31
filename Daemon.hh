#ifndef DAEMON_HH
#define DAEMON_HH

#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <unistd.h>

#include "DomainSocket.hh"

class Daemon
{
  public:
	Daemon (const std::string & path, bool mode);
	 ~Daemon (void);

	void start (void);

  private:
	int initSelect (struct timeval *tv, fd_set * readfds,
					fd_set * writefds);
	void handleSockets (void);
	void eventTerminal (void);
	void eventServer (void);
	void eventClients (fd_set * readfds, fd_set * writefds);

  private:
	DomainSocket mDomainSocket;
	bool mInit, mMode;			/* mMode: true NO-Blocking */
	std::vector < DomainSocket * > mClients;
};

#endif							// DAEMON_HH
