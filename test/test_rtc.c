#include <stdio.h>
#include "ldal_rtc.h"

static struct ldal_rtc_device rtc0 =
{
    "rtc0",
    "/dev/rtc0",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    struct ldal_rtc_time rt;

    printf("RTC Test Start\n");

    /* Register class */
    ret = rtc_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register rtc class failed\n");
    }

    /* Register device */
    ret = ldal_device_register(&rtc0.device, rtc0.device_name, rtc0.file_name, LDAL_CLASS_RTC, (void *) &rtc0);
    if (ret != LDAL_EOK) {
        printf("Register rtc device failed\n");
    }

    /* Get device handler */
    device = ldal_device_get_by_name("rtc0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s\n", device->name);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init rtc device failed\n");
        return -1;
    }

    /* Storage to struct */
    ret = read_device(device, (char *)&rt, sizeof(rt));
    if (ret != LDAL_EOK) {
        printf("Read rtc device failed\n");
        goto __exit;
    }
    
    printf("[%s] Now: %d-%02d-%02d %02d:%02d:%02d (%d)\n", device->name, 
            rt.year, rt.mon, rt.mday, rt.hour, rt.min, rt.sec, rt.wday);
    
    rt.year += 1;
    rt.hour = rt.hour < 16 ? rt.hour + 8 : rt.hour -16;
    ret = write_device(device, (char *)&rt, sizeof(rt));
    if (ret != LDAL_EOK) {
        printf("Write rtc device failed\n");
        goto __exit;
    }

    sleep(1);

    /* Storage to array */
    int dt[7];
    ret = read_device(device, (char *)&dt, sizeof(dt));
    if (ret != LDAL_EOK) {
        printf("Read rtc device failed\n");
        goto __exit;
    }

    printf("[%s] Now: %d-%02d-%02d %02d:%02d:%02d (%d)\n", device->name, 
            dt[5], dt[4], dt[3], dt[2], dt[1], dt[0], dt[6]);

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK) {
        printf("Stop rtc device failed\n");
    }

    printf("RTC Test End\n");
    return 0;
}
