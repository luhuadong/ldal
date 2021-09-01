#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table device_table[] = {
    { "rtc0", "/dev/rtc0", LDAL_CLASS_RTC },
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    struct ldal_rtc_time rt;

    printf("RTC Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));
    
    ldal_show_device_list();

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
