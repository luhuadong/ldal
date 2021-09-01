#include <stdio.h>
#include "ldal.h"

#define MESSAGE_STRING  "Make the most of everyday."

static struct ldal_device_table device_table[] = {
    { "ec25", "/dev/ttyUSB2", LDAL_CLASS_ME },
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;

    printf("EC25 Test Start\n");

    /* Register device */
    ldal_device_create(&device_table, ARRAY_SIZE(device_table));

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("ec25");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init ec25 device failed\n");
        return -1;
    }

    /* Send a message */
    ret = write_device(device, MESSAGE_STRING, strlen(MESSAGE_STRING)+1);
    if (ret != LDAL_EOK) {
        printf("Write me device failed\n");
    }

    ret = stop_device(device);
    if (ret != LDAL_EOK) {
        printf("Stop me device failed\n");
    }

    printf("EC25 Test End\n");
    return 0;
}
