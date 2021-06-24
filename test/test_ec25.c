#include <stdio.h>
#include "ldal_me.h"

#define MESSAGE_STRING  "Make the most of everyday."

static struct ldal_me_device ec25 = {
    "ec25",
    "/dev/ttyUSB2",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;

    printf("EC25 Test Start\n");

    /* Register device */
    ret = ldal_device_register(&ec25.device, ec25.device_name, ec25.file_name, LDAL_CLASS_ME, (void *)&ec25);
    if (ret != LDAL_EOK) {
        printf("Register me device failed\n");
    }

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
