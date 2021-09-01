#include <stdio.h>
#include "ldal_misc.h"

static int misc_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR | O_NONBLOCK);
    if (device->fd < 0)
    {
        perror("Can't open misc device");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int misc_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int misc_read(struct ldal_device *device, void *buf, size_t len)
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

static int misc_write(struct ldal_device *device, const void *buf, size_t len)
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

const struct ldal_device_ops misc_device_ops = 
{
    .open  = misc_open,
    .close = misc_close,
    .read  = misc_read,
    .write = misc_write,
};

int misc_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for misc device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_MISC;
    class->device_ops = &misc_device_ops;

    debug_print("Register misc class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_MISC);
}
INIT_CLASS_EXPORT(misc_device_class_register);
