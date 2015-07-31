/**
* @file epoll_base.h
* @brief Definition of epoll create, epoll_ctl, epoll_del.
*/

#ifndef		YEALINK_EPOLL_H
#define		YEALINK_EPOLL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined (__cplusplus)
extern "C"
{
#endif

   int osal_epoll_create (int size);
   int osal_epoll_ctl (int epfd, int op, int fd,
			     struct epoll_event *event);
   int osal_epoll_wait (int epfd, struct epoll_event *events,
			      int maxevents, int timeout);

   int osal_epoll_out_register (int epoll_fd, int fd);
   int osal_epoll_mod_out_register (int epoll_fd, int fd);
   int osal_epoll_in_register (int epoll_fd, int fd);
   int osal_epoll_mod_in_register (int epoll_fd, int fd);
   int osal_epoll_deregister (int epoll_fd, int fd);


#if defined (__cplusplus)
}
#endif

#endif
