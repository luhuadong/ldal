#include <stdio.h>
#include "ldal_file.h"

static int file_open(struct ldal_device *device)
{
    assert(device);

    //FILE *fp;

    device->fd = open(device->filename, O_RDWR | O_CREAT, 0644);
    if (device->fd < 0)
    {
        perror("Can't open regular file");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int file_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int file_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    lseek(device->fd, 0, SEEK_SET);
    ret = read(device->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }
    
    return LDAL_EOK;
}

static int file_write(struct ldal_device *device, const void *buf, size_t len)
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

const struct ldal_device_ops file_device_ops = 
{
    .open  = file_open,
    .close = file_close,
    .read  = file_read,
    .write = file_write,
};


int file_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for file device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_FILE;
    class->device_ops = &file_device_ops;

    debug_print("Register file class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_FILE);
}
INIT_CLASS_EXPORT(file_device_class_register);
