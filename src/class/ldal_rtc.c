#include <time.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "ldal_rtc.h"

static int rtc_open(struct ldal_device *device)
{
    assert(device);

    device->fd = open(device->filename, O_RDWR);
    if (device->fd < 0)
    {
        perror("Can't Open RTC");
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int rtc_close(struct ldal_device *device)
{
    assert(device);
    close(device->fd);
    return LDAL_EOK;
}

static int rtc_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    struct rtc_time t;
    struct ldal_rtc_time rt;
    size_t size = sizeof(struct ldal_rtc_time);
    
    /* ioctl command RTC_RD_TIME is used to read the current timer */
    ret = ioctl(device->fd, RTC_RD_TIME, &t);
    if (ret < 0) {
        perror("rtc ioctl RTC_RD_TIME error");
        return -LDAL_ERROR;
    }

    rt.sec  = t.tm_sec;
    rt.min  = t.tm_min;
    rt.hour = t.tm_hour;
    rt.mday = t.tm_mday;
    rt.mon  = t.tm_mon + 1;
    rt.year = t.tm_year + 1900;
    rt.wday = t.tm_wday;

    if (len < sizeof(struct ldal_rtc_time)) {
        size = len;
    }
    memcpy(buf, &rt, size);
    return LDAL_EOK;
}

static int rtc_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);
    assert(buf);

    int ret;
    struct rtc_time t;
    struct ldal_rtc_time *rt;

    if (len < sizeof(struct ldal_rtc_time)) {
        return -LDAL_ERROR;
    }
    rt = (struct ldal_rtc_time *)buf;

    t.tm_sec  = rt->sec;
    t.tm_min  = rt->min;
    t.tm_hour = rt->hour;
    t.tm_mday = rt->mday;
    t.tm_mon  = rt->mon - 1;
    t.tm_year = rt->year - 1900;
    t.tm_wday = rt->wday;

    ret = ioctl(device->fd, RTC_SET_TIME, &t);
    if (ret < 0) {
        perror("rtc ioctl RTC_SET_TIME error");
        return -LDAL_ERROR;
    }
    return LDAL_EOK;
}

const struct ldal_device_ops rtc_device_ops = 
{
    .open  = rtc_open,
    .close = rtc_close,
    .read  = rtc_read,
    .write = rtc_write,
};

int rtc_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for rtc device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_RTC;
    class->device_ops = &rtc_device_ops;

    debug_print("Register rtc class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_RTC);
}
INIT_CLASS_EXPORT(rtc_device_class_register);
