#include "DomainSocket.hh"

DomainSocket::DomainSocket (const std::string & path, DomainSocket::SocketType type, bool mode): mInit (false), mSocketType (type), mAcceptMax (5), mSocketMode(mode)
{
	// Prepare the socket structure
	mSocket.sun_family = AF_UNIX;
	::strncpy (mSocket.sun_path, path.c_str (), path.length ());

	// Get new socket fd
	if ((mSocketFd =::socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error (strerror (errno));

	 /* important: make the fd non-blocking */
	if (mSocketMode) 
	{
        int flags;

		flags =  fcntl(mSocketFd, F_GETFL, 0);
		if ( flags == -1 ){
			ERR_LOG;	
		    throw std::runtime_error (strerror (errno));
		}
		if (fcntl(mSocketFd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			ERR_LOG;	
		    throw std::runtime_error (strerror (errno));
		}  
	}

	// Server Socket
	if (mSocketType == DomainSocket::SERVER)
	{
		// Remove old socket if exist
		::unlink (path.c_str ());

		// Connect the socket and start listening
		if (::bind (mSocketFd, (struct sockaddr *) &mSocket,
					path.length () + sizeof (mSocket.sun_family)) == -1)
		{
			throw std::runtime_error (strerror (errno));

		} else if (::listen (mSocketFd, mAcceptMax) == -1)
		{	  
			throw std::runtime_error (strerror (errno));
        } else {
			mInit = true;
		}
	} else if (mSocketType == DomainSocket::CLIENT)	// Client Socket
	{
		// Connect to the remote socket (a SERVER type socket)
		if (::connect (mSocketFd, (struct sockaddr *) &mSocket,
					   path.length () + sizeof (mSocket.sun_family)) == -1) 
		{
			throw std::runtime_error (strerror (errno));
		} else {
			mInit = true;
		}
	} else {
		throw std::runtime_error("Wrong constructor for a Server_Client socket.");
	}
}

DomainSocket::DomainSocket (int fd, const sockaddr_un * socket, bool mode):mInit (true), mSocketType (DomainSocket::SERVER_CLIENT), mSocketFd (fd), mSocketMode(mode)
{
	// Copy the content of the socket structure
	::memcpy (&mSocket, socket,
			  strlen (socket->sun_path) + sizeof (socket->sun_family));
	
	/* important: make the fd non-blocking */
	if (mSocketMode) 
	{
        int flags;

		flags =  fcntl(mSocketFd, F_GETFL, 0);
		if ( flags == -1 ){
			ERR_LOG;	
		    throw std::runtime_error (strerror (errno));
		}
		if (fcntl(mSocketFd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			ERR_LOG;	
		    throw std::runtime_error (strerror (errno));
		}  
	}
}

DomainSocket::~DomainSocket (void)
{
	if (mInit)
		::close (mSocketFd);
}

DomainSocket *
DomainSocket::acceptClient (void)
{
	int remoteFd;
	struct sockaddr_un remoteSocket;
	socklen_t size = sizeof (remoteSocket);

	// Only SERVER type socket can accept clients
	if (mSocketType != DomainSocket::SERVER)
		throw std::runtime_error ("This is not a server socket");

	// Accept the new client
	if ((remoteFd =::accept (mSocketFd, (struct sockaddr *) &remoteSocket,
							 &size)) == -1)
		throw std::runtime_error (strerror (errno));

	// Create a SERVER_CLIENT socket to handle the client
	return new DomainSocket (remoteFd, &remoteSocket, true);
}

/**********************************************************
 * when mode is true, flags shoud be set to MSG_DONTWAIT  *
 **********************************************************
 */
 
ssize_t 
DomainSocket::sendData (const void *buf, size_t len, int flags)
{
	// SERVER type socket send data to clients via SERVER_CLIENT socket
	if (mSocketType == DomainSocket::SERVER)
		throw std::runtime_error ("This is a server socket");

	//if (send (mSocketFd, buf, len, 0) == -1)
	//	throw std::runtime_error (strerror (errno));

	// Send the data
    return send(mSocketFd, buf, len, flags);
}

/**********************************************************
 * when mode is true, flags shoud be set to MSG_DONTWAIT  *
 **********************************************************
 */

ssize_t 
DomainSocket::recvData (void *buf, size_t len, int flags)
{
	// SERVER type socket receive msg to clients via SERVER_CLIENT socket
	if (mSocketType == DomainSocket::SERVER)
		throw std::runtime_error ("This is a server socket");

	/*// Receive the data
	if ((len = recv (mSocketFd, buf, len, 0)) == -1) 
	{	  
		throw std::runtime_error (strerror (errno));
	} else if (len == 0) {					// Client disconnected
		throw DomainSocket::Disconnected ();
	}
	ret.assign (buf, len);
	return ret;
	*/

	return recv(mSocketFd, buf, len, flags);
}

void
DomainSocket::closeSocket (void)
{
	if (mInit)
		close (mSocketFd);
}

bool
DomainSocket::initCheck (void) const 
{
    return mInit;
}

int 
DomainSocket::getSocketFd (void) const
{
	return mSocketFd;
}

const sockaddr_un *
DomainSocket::getSocket (void) const
{
    return &mSocket;
}
