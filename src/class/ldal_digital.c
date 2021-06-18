#include <stdio.h>
#include "ldal_digital.h"

#define DO_CONF_PLUSE_PERIOD    _IOW('k',2,int)
#define DI_GET_PLUSE_COUNT      _IOW('k',3,int)
#define DI_CLR_PLUSE_COUNT      _IOW('k',4,int)

static int digital_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR);
    if (device->fd < 0)
    {
        perror("Can't open digital port");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int digital_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int digital_read(struct ldal_device *device, void *buf, size_t len)
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

static int digital_write(struct ldal_device *device, const void *buf, size_t len)
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

const struct ldal_device_ops digital_device_ops = 
{
    .open  = digital_open,
    .close = digital_close,
    .read  = digital_read,
    .write = digital_write,
};

int digital_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for digital device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_DIGITAL;
    class->device_ops = &digital_device_ops;

    printf("Register digital device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_DIGITAL);
}
INIT_CLASS_EXPORT(digital_device_class_register);
