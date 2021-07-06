#include <stdio.h>
#include <sys/ioctl.h>
#include "ldal_digital.h"

#define DO_CONF_PLUSE_PERIOD    _IOW('k',2,int)
#define DI_GET_PLUSE_COUNT      _IOW('k',3,int)
#define DI_CLR_PLUSE_COUNT      _IOW('k',4,int)

static int digital_open(struct ldal_device *dev)
{
    assert(dev);

    dev->fd = open(dev->filename, O_RDWR | O_NONBLOCK);
    if (dev->fd < 0)
    {
        perror("Can't open digital port");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int digital_close(struct ldal_device *dev)
{
    assert(dev);
    close(dev->fd);
    return LDAL_EOK;
}

static int digital_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    ret = read(dev->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int digital_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    int ret;
    ret = write(dev->fd, buf, len);
    if (ret == -1) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int digital_control(struct ldal_device *dev, int cmd, void *arg)
{
    assert(dev);
    int ret = 0;

    switch(cmd) {
    case DI_GET_PLUSE_COUNT: 
    {
        long count=0;
        ret = ioctl(dev->fd, cmd, &count);
        if(ret < 0) {
            count = 0;
            printf("Get DI pluse count error, ret = %d\n", ret);
            return -LDAL_ERROR;
        }
        memcpy((long *)arg, &count, sizeof(count));
        return LDAL_EOK;
    } break;

    case DI_CLR_PLUSE_COUNT: 
    {
        ret = ioctl(dev->fd, cmd, NULL);
        if(ret < 0) {
            printf("Clear DI pluse count error, ret = %d\n", ret);
            return -LDAL_ERROR;
        }     
    } break;

    case DO_CONF_PLUSE_PERIOD : 
    {
        ret = ioctl(dev->fd, cmd, (unsigned short *)arg);
        if(ret<0) {
            printf("DO pluse output error, ret = %d\n",ret);
            return -LDAL_ERROR;
        }  
    } break;

    default: 
        ret = -LDAL_EINVAL;
        break;
    }

    return LDAL_EOK;
}

const struct ldal_device_ops digital_device_ops = 
{
    .open  = digital_open,
    .close = digital_close,
    .read  = digital_read,
    .write = digital_write,
    .control = digital_control,
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
