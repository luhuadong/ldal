#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ldal_udp.h"

static int udp_open(struct ldal_device *device)
{
    assert(device);

    int s;
    struct sockaddr_in addr;
    char buffer[256];

    device->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(device->fd < 0) {
        perror("Can't create socket");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int udp_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int udp_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    assert(buf);

    /* 
    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, 
                     struct sockaddr *src_addr, socklen_t *addrlen);

    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
    */

    int ret;
    ret = read(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int udp_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);
    assert(buf);

    /* 
    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen);

    ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
    */

    int ret;
    ret = write(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int udp_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);

    return LDAL_EOK;
}

const struct ldal_device_ops udp_device_ops = 
{
    .open  = udp_open,
    .close = udp_close,
    .read  = udp_read,
    .write = udp_write,
    .control = udp_control,
};

int udp_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for UDP device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_UDP;
    class->device_ops = &udp_device_ops;

    printf("Register socket device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_UDP);
}
INIT_CLASS_EXPORT(udp_device_class_register);
