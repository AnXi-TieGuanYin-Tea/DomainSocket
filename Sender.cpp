#include "Sender.hh"

Sender::Sender(const std::string &path, void *data, ssize_t len, bool mode)
    : mDomainSocket(path, DomainSocket::CLIENT, mode)
{
    if (mDomainSocket.initCheck())
    {
        try
        {
            mDomainSocket.sendData(data, len, MSG_DONTWAIT);
            std::cout << data << " : Sended to server !" << std::endl;
           // std::cout << "Server response : "  << std::endl;
        }
        catch (std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
        } 
}
}

int main(int ac, char **av)
{
	char tempbuf[1280];
	memset(tempbuf, 1, sizeof(tempbuf));

    if (ac != 3)
    {
        std::cout << "Usage : " << av[0] << " <socket_path> <msg>" << std::endl;
        return 1;
    }

    try
    {
        Sender      s(av[1], (void *)tempbuf, sizeof(tempbuf), true);
    }

	    catch (std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
		while(1)
	{
	 int i = 1;
	}



    return 0;
}
