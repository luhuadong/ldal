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
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include "ldal_tcp.h"

static char linktype[LINK_TYPE_LEN+1][20]={
    "tcp client",
    "tcp server",
};

static char linkstat[LINK_STAT_LEN+1][20] = {
    "initial",
    "connect",
    "connect error",
    "disconnect",
    "disconnect error",
    "end"
};

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

static int tcp_bind_local_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
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

static void set_server_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
{
    assert(dev);
    assert(ipaddr);

    struct ldal_tcp_device *link = (struct ldal_tcp_device *)dev->user_data;

    link->server.sin_family = AF_INET;
    link->server.sin_port = htons(port);
    link->server.sin_addr.s_addr = inet_addr(ipaddr);

    sprintf(dev->filename, "%s:%u", ipaddr, port);
}

static int tcp_connect_server_addr(struct ldal_device *dev, const char *ipaddr, const uint16_t port)
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

    set_server_addr(dev, ipaddr, port);
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

    set_server_addr(dev, ipaddr, atoi(port));

    return LDAL_EOK;
}

static int tcp_init(struct ldal_device *dev)
{
    assert(dev);

    //struct ldal_tcp_device *link = (struct ldal_tcp_device *)dev->user_data;

    dev->fd = socket(AF_INET, SOCK_STREAM, 0);
    if(dev->fd < 0) {
        perror("Can't create socket");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int tcp_open(struct ldal_device *dev)
{
    assert(dev);

    //struct sockaddr_in addr;

    import_ipaddr_from_filename(dev);

    return LDAL_EOK;
}

static int tcp_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int tcp_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    //struct ldal_tcp_device *link = (struct ldal_tcp_device *)dev->user_data;
    
    ret = recv(dev->fd, buf, len, 0);
    if (ret == -1) {
        return -errno;
    }

    return ret;
}

static int tcp_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    //struct ldal_tcp_device *link = (struct ldal_tcp_device *)dev->user_data;
    //socklen_t addr_len = sizeof(struct sockaddr);

    ret = send(dev->fd, buf, len, 0);
    if (ret == -1) {
        return -errno;
    }

    return ret;
}

static int tcp_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);

    int ret = 0;
    struct ldal_tcp_device *link = (struct ldal_tcp_device *)dev->user_data;

    switch(cmd) {
    case SOCKET_SET_REUSEADDR: 
    {
        set_reuse_addr(dev);
        break;
    }
    case SOCKET_BINDTODEVICE: 
    {
        char *ifname = (char *)arg;
        bind_to_device(dev, ifname);
        break;
    }
    case SOCKET_SET_KEEPALIVE : 
    {
        break;
    }
    case SOCKET_SET_RECVTIMEO :
    {
        link->recv_timeout = (uint32_t)arg;
        struct timeval tv;
        tv.tv_sec = link->recv_timeout / 1000;
        tv.tv_usec = (link->recv_timeout % 1000) * 1000;
        setsockopt(dev->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        break;
    }
    case SOCKET_SET_SENDTIMEO :
    {
        link->send_timeout = (uint32_t)arg;
        struct timeval tv;
        tv.tv_sec = link->send_timeout / 1000;
        tv.tv_usec = (link->send_timeout % 1000) * 1000;
        setsockopt(dev->fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
        break;
    }
    case SOCKET_SET_ETHDEV :
    {
        break;
    }
    default: 
        ret = -LDAL_EINVAL;
        break;
    }

    return ret;
}

const struct ldal_device_ops tcp_device_ops = 
{
    .init    = tcp_init,
    .open    = tcp_open,
    .close   = tcp_close,
    .read    = tcp_read,
    .write   = tcp_write,
    .control = tcp_control,
    .bind    = tcp_bind_local_addr,
    .connect = tcp_connect_server_addr,
};

int tcp_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for tcp device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_TCP;
    class->device_ops = &tcp_device_ops;

    printf("Register tcp device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_TCP);
}
INIT_CLASS_EXPORT(tcp_device_class_register);
