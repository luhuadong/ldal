#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include "ldal_udp.h"


/**
 * This function will set a socket to SO_REUSEADDR.
 *
 * @param sockfd the socket referred to by the file descriptor sockfd.
 *
 * @return On success, zero is returned for the standard options.  On error, -1 is returned, and errno is set appropriately.
 */
static int set_reuse_addr(struct ldal_device *dev)
{
    assert(dev);

    int optval = 1;
    int optlen = sizeof(optval);
    return setsockopt(dev->fd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
}

static int bind_to_device(struct ldal_device *dev, const char *netdev)
{
    struct ifreq ifr;
    int ret=0;

    /* unbind network device (eth0 eth1 ppp0) from sock */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "", 1);
    ret = setsockopt(dev->fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    if(ret < 0) {
        return -LDAL_ERROR;
    }
    
    /* rebind network device (eth0 eth1 ppp0) from sock */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, netdev, strlen(netdev)+1);
    ret = setsockopt(dev->fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));   
    if(ret < 0) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int udp_bind_local_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
{
    assert(dev);

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(struct sockaddr_in));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    if (NULL == ipaddr) {
        saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        saddr.sin_addr.s_addr = inet_addr(ipaddr);
    }

    return bind(dev->fd, (struct sockaddr*)&saddr, sizeof(struct sockaddr));
}

static void set_remote_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
{
    assert(dev);

    struct ldal_udp_device *link = (struct ldal_udp_device *)dev->user_data;

    link->saddr.sin_family = AF_INET;
    link->saddr.sin_port = htons(port);
    link->saddr.sin_addr.s_addr = inet_addr(ipaddr);

    sprintf(dev->filename, "%s:%u", ipaddr, port);
}

static int udp_connect_server_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
{
    assert(dev);

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(struct sockaddr_in));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ipaddr);

    if (-1 == connect(dev->fd, (struct sockaddr* )&saddr, sizeof(struct sockaddr))) {
        return -LDAL_ERROR;
    }

    set_remote_addr(dev, ipaddr, port);
    return LDAL_EOK;
}

static int import_ipaddr_from_filename(struct ldal_device *dev)
{
    assert(dev);

    if (dev->filename == NULL) {
        return -LDAL_EINVAL;
    }

    char *ipaddr = NULL;
    char *port   = NULL;
    char *delim  = ":";
    char ipstr[LDAL_FILENAME_MAX];
    memcpy(ipstr, dev->filename, sizeof(dev->filename));

    ipaddr = strtok(ipstr, delim);
    port = strtok(NULL, delim);

    if (ipaddr == NULL || port == NULL) {
        return -LDAL_EINVAL;
    }

    set_remote_addr(dev, ipaddr, atoi(port));

    return LDAL_EOK;
}

static int udp_init(struct ldal_device *dev)
{
    assert(dev);

    struct ldal_udp_device *link = (struct ldal_udp_device *)dev->user_data;
    link->echo_flag = false;

    return LDAL_EOK;
}

static int udp_open(struct ldal_device *dev)
{
    assert(dev);

    //struct sockaddr_in addr;

    dev->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(dev->fd < 0) {
        perror("Can't create socket");
        return -LDAL_ERROR;
    }

    import_ipaddr_from_filename(dev);

    return LDAL_EOK;
}

static int udp_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int udp_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    struct ldal_udp_device *link = (struct ldal_udp_device *)dev->user_data;
    socklen_t addr_len = sizeof(struct sockaddr);
    
    ret = recvfrom(dev->fd, buf, len, 0, (struct sockaddr*)&link->raddr, &addr_len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    if (link->echo_flag) {
        memcpy(&link->saddr, &link->raddr, addr_len);
    }

    return LDAL_EOK;
}

static int udp_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    struct ldal_udp_device *link = (struct ldal_udp_device *)dev->user_data;
    socklen_t addr_len = sizeof(struct sockaddr);

    //ret = send(dev->fd, buf, len, 0);
    ret = sendto(dev->fd, buf, len, 0, (struct sockaddr*)&link->saddr, addr_len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int udp_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);

    int ret = 0;
    struct ldal_udp_device *link = (struct ldal_udp_device *)dev->user_data;

    switch(cmd) {
    case SOCKET_SET_REUSEADDR: 
    {
        set_reuse_addr(dev);
        break;
    }
    case SOCKET_SET_NETDEV: 
    {
        char *ifname = (char *)arg;
        bind_to_device(dev, ifname);
        break;
    }
    case SOCKET_GET_RECVADDR :
    {
        if (arg) {
            memcpy(arg, &link->raddr, sizeof(link->raddr));
            ret = LDAL_EOK;
        }
        ret = -LDAL_EINVAL;
        break;
    }
    case SOCKET_SET_ECHO_FLAG :
    {
        link->echo_flag = (bool)arg;
        ret = LDAL_EOK;
        break;
    }
    default: 
        ret = -LDAL_EINVAL;
        break;
    }

    return ret;
}

const struct ldal_device_ops udp_device_ops = 
{
    .init    = udp_init,
    .open    = udp_open,
    .close   = udp_close,
    .read    = udp_read,
    .write   = udp_write,
    .control = udp_control,
    .bind    = udp_bind_local_addr,
    .connect = udp_connect_server_addr,
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

    debug_print("Register udp class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_UDP);
}
INIT_CLASS_EXPORT(udp_device_class_register);
