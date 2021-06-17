#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ldal_gpio.h"

static int socket_open(struct ldal_device *device)
{
    assert(device);

    int s;
    struct sockaddr_in addr;
    char buffer[256];

    device->fd = socket(AF_INET, SOCK_STREAM, 0);
    if(device->fd < 0) {
        perror("Can't create socket");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int socket_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int socket_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    ret = read(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int socket_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    ret = write(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

const struct ldal_device_ops socket_device_ops = 
{
    .open  = socket_open,
    .close = socket_close,
    .read  = socket_read,
    .write = socket_write,
};

int socket_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for gpio device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_SOCKET;
    class->device_ops = &socket_device_ops;

    printf("Register socket device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_SOCKET);
}
INIT_CLASS_EXPORT(socket_device_class_register);
