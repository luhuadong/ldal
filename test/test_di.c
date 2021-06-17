#include <stdio.h>
#include "ldal_digital.h"

static struct ldal_digital_device di0 = {
    "DI0",
    "/dev/didev0",
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;

    printf("Digital Port Test Start\n");

    /* Register class */
    ret = digital_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register digital class failed\n");
    }

    /* Register device */
    ret = ldal_device_register(&di0.device, di0.device_name, di0.file_name, LDAL_CLASS_DIGITAL, (void *)&di0);
    if (ret != LDAL_EOK) {
        printf("Register digital device failed\n");
    }

    /* Get device handler */
    device = ldal_device_get_by_name("DI0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init digital device failed\n");
        return -1;
    }

    printf("Reading digital port ...\n");

    for (int i = 0; i < 5; i++) {
        
        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read digital device failed\n");
            goto __exit;
        }
        printf("> %d\n", value);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop digital device failed\n");
    }

    printf("Digital Port Test End\n");
    return 0;
}
