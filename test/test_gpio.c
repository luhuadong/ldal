#include <stdio.h>
#include "ldal_gpio.h"

static struct ldal_gpio_device gpio0 = {
    "gpio0",
    "/dev/uart-power",
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;

    printf("GPIO Test Start\n");

    /* Register class */
    ret = gpio_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register file class failed\n");
    }

    /* Register device */
    ret = ldal_device_register(&gpio0.device, gpio0.device_name, gpio0.file_name, LDAL_CLASS_GPIO, (void *)&gpio0);
    if (ret != LDAL_EOK) {
        printf("Register gpio device failed\n");
    }

    /* Get device handler */
    device = ldal_device_get_by_name("gpio0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init gpio device failed\n");
        return -1;
    }

    for (int i = 0; i < 5; i++) {
        printf("Writing gpio ...\n");

        value = !value;
        ret = write_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Write gpio device failed\n");
            goto __exit;
        }

        usleep(500 * 1000);

        printf("Reading gpio ...\n");

        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read gpio device failed\n");
            goto __exit;
        }
        printf("> %d\n", value);
        usleep(500 * 1000);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop gpio device failed\n");
    }

    printf("GPIO Test End\n");
    return 0;
}
