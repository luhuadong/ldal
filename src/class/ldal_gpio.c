#include <stdio.h>
#include "ldal_gpio.h"

static int gpio_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR);
    if (device->fd < 0)
    {
        perror("Can't open gpio");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int gpio_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int gpio_read(struct ldal_device *device, void *buf, size_t len)
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

static int gpio_write(struct ldal_device *device, const void *buf, size_t len)
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

const struct ldal_device_ops gpio_device_ops = 
{
    .open  = gpio_open,
    .close = gpio_close,
    .read  = gpio_read,
    .write = gpio_write,
};

int gpio_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for gpio device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_GPIO;
    class->device_ops = &gpio_device_ops;

    debug_print("Register gpio class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_GPIO);
}
INIT_CLASS_EXPORT(gpio_device_class_register);
