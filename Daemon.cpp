#include "Daemon.hh"

Daemon::Daemon (const std::string & path, bool mode):mDomainSocket(path, DomainSocket::SERVER, mode), mInit(false), mMode(mode)
{
}

Daemon::~Daemon (void)
{
	for (unsigned int i = 0; i < mClients.size (); ++i)
	{
		delete mClients[i];
	}
}

void
Daemon::start (void)
{
	mInit = true;
	while (mInit)
	{
		handleSockets ();
	}
	std::cout << "Server shutdown" << std::endl;
}

void
Daemon::handleSockets (void)
{
	fd_set readfds;
	fd_set writefds;
	int fd_max;
	struct timeval tv;

	fd_max = initSelect (&tv, &readfds, &writefds);
	if (::select (fd_max, &readfds, NULL, NULL, &tv) == -1)
	{
		throw std::runtime_error (std::string ("Select Error : ") +::
								  strerror (errno));
	}
	else
	{
		// If something to read on stdin
		if (FD_ISSET (0, &readfds))
			eventTerminal ();
		// If new client connect
		if (FD_ISSET (mDomainSocket.getSocketFd(), &readfds))
			eventServer ();
		// Check clients's socket
		eventClients (&readfds, &writefds);
	}

}

int
Daemon::initSelect (struct timeval *tv, fd_set * readfds,
					fd_set * writefds)
{
	int fd_max = mDomainSocket.getSocketFd();

	// Timeout 100 ms
	tv->tv_sec = 0;
	tv->tv_usec = 100;

	// Initialize bits field for select
	FD_ZERO (readfds);
	FD_SET (mDomainSocket.getSocketFd(), readfds);
	FD_SET (0, readfds);
	if (writefds != NULL)
	{
		FD_ZERO (writefds);
		FD_SET (mDomainSocket.getSocketFd(), writefds);
	}
	for (unsigned int i = 0; i < mClients.size (); ++i)
	{
		FD_SET (mClients[i]->getSocketFd(), readfds);
		if (writefds != NULL)
			FD_SET (mClients[i]->getSocketFd(), writefds);
		// Check if client's fd is greater than actual fd_max
		fd_max = (fd_max < mClients[i]->getSocketFd())? mClients[i]->getSocketFd() : fd_max;
	}
	return fd_max + 1;
}

void
Daemon::eventTerminal (void)
{
	std::string msg;

	std::cin >> msg;
	if (msg == "exit")
	{
		mInit = false;
	}
}

void
Daemon::eventServer (void)
{
	DomainSocket *client;

	try
	{
		client = mDomainSocket.acceptClient ();
		mClients.push_back (client);
	}
	catch (std::runtime_error & e)
	{
		std::cout << e.what () << std::endl;
	}
}

void
Daemon::eventClients (fd_set * readfds, fd_set * writefds)
{
	std::string msg;
	char tempBuf1[1280], tempBuf2[1280];
    int t = 0, recvBytest, sendBytes;
	memset(tempBuf1, 0, sizeof(tempBuf1));
	memset(tempBuf2, 0, sizeof(tempBuf2));

   
	for (std::vector < DomainSocket * >::iterator it = mClients.begin ();
		 it < mClients.end (); ++it)
	{
#if 0
		// Something to write on client socket
		if (FD_ISSET ((*it)->getSocketFd(), writefds))
		{
		  try
			 {
				t= 1;
				int i = 0;
				do {
					DBG("abc");
				    sendBytes = (*it)->sendData((void *)tempBuf1, sizeof(tempBuf1), MSG_DONTWAIT);
					DBG("sendBytes:%d\n", sendBytes);
				 }while (sendBytes < sizeof(tempBuf1) && i < 8);
			   }
				catch (std::runtime_error & e)
				{
					std::cout << e.what () << std::endl;
				}
		}
#endif
		// Something to read on client socket
		if (FD_ISSET ((*it)->getSocketFd(), readfds))
		{
			try
			{
				int j = 0;
				do {
				recvBytest = (*it)->recvData((void *)tempBuf2, sizeof(tempBuf2), MSG_DONTWAIT);
				if (recvBytest > 0)
				DBG("recvBytest:%d\n", recvBytest);
				}while(recvBytest < sizeof(tempBuf2) && j++ < 8);
			}
			catch (DomainSocket::Disconnected & e)
			{
				delete (*it);
				mClients.erase (it);
				it = mClients.begin ();
			}
			catch (std::runtime_error & e)
			{
				std::cout << e.what () << std::endl;
			}
		}
	}
	if (t == 0){
	//  DBG("No Hava Data!\n");
	  t = 1;
	}
}

int
main (int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Usage : " << av[0] << " <socket_path>" << std::endl;
		return 1;
	}

	try
	{
		Daemon d (av[1], true);

		d.start ();
	}
	catch (std::runtime_error & e)
	{

	}

	return 0;
}
