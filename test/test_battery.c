#include <stdio.h>
#include "ldal_misc.h"

static struct ldal_misc_device battery = {
    "battery",
    "/dev/k37adev_battery",
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;

    printf("Battery Test Start\n");

    /* Register device */
    ret = ldal_device_register(&battery.device, battery.device_name, battery.file_name, LDAL_CLASS_MISC, (void *)&battery);
    if (ret != LDAL_EOK) {
        printf("Register misc device failed\n");
    }

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("battery");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init misc device failed\n");
        return -1;
    }

    printf("Reading battery power ...\n");
    for (int i = 0; i < 5; i++) {

        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read misc device failed\n");
            goto __exit;
        }
        printf("> %d %%\n", value);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop misc device failed\n");
    }

    printf("Battery Test End\n");
    return 0;
}
