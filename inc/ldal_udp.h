#ifndef __LDAL_DEVICE_UDP_H__
#define __LDAL_DEVICE_UDP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ldal.h"


#define SOCKET_SET_REUSEADDR     0x101
#define SOCKET_BINDTODEVICE      0x102
#define SOCKET_SET_NETMASK       0x103
#define SOCKET_GET_RECVADDR      0x104
#define SOCKET_SET_ECHO_FLAG     0x105


struct ldal_udp_device
{
    char *device_name;
    char *file_name;
    int status;

    struct sockaddr_in saddr;    /* send addr */
    struct sockaddr_in raddr;    /* recv addr */
    bool echo_flag;

    struct ldal_device device;
    void *user_data;
};

int udp_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_UDP_H__ */