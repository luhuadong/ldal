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
#include "ldal_private.h"
#include "ldal.h"

struct ldal_udp_device
{
    struct ldal_device device;
    int status;

    struct sockaddr_in saddr;    /* send addr */
    struct sockaddr_in raddr;    /* recv addr */
    bool echo_flag;

    void *user_data;
};

int udp_device_class_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_DEVICE_UDP_H__ */