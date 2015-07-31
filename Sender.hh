
#ifndef SENDER_HH
#define SENDER_HH

#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>

#include "DomainSocket.hh"

class Sender
{
private:
    DomainSocket            mDomainSocket;

public:
    Sender(const std::string &path, void *data, ssize_t len, bool mode);
};

#endif // SENDER_HH
