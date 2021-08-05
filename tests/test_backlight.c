#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table device_table[] = {
    { "bl0", "/sys/class/backlight/rgb_backlight", LDAL_CLASS_BACKLIGHT },
};

int main(int argc, char *argv[])
{
    int ret;
    int value, old_value;
    struct ldal_device *device;

    printf("Backlight Test Start\n");

    /* Register device */
    ldal_device_create(&device_table, ARRAY_SIZE(device_table));

    /* Get device handler */
    device = ldal_device_get_by_name("bl0");
    if (device == NULL) {
        printf("Can't get device\n");
        return -1;
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init backlight device failed\n");
        return -1;
    }

    read_device(device, (void *)&old_value, sizeof(old_value));
    printf("Current Backlight Level: %d\n", old_value);

    for (int i=0; i <= 100; i++) {
        value = i;

        printf("---------\nset %d\n", value);
        write_device(device, (void *)&value, sizeof(value));
        usleep(100 * 1000);
    }

    sleep(2);
    printf("---------\nreset %d\n", old_value);
    write_device(device, (void *)&old_value, sizeof(old_value));

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop backlight device failed\n");
    }

    printf("Backlight Test End\n");
    return 0;
}
