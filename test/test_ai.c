#include <stdio.h>
#include "ldal.h"

static struct ldal_analog_device ai0 = {
    "AI0",
    "/dev/aidev0",
    0,
};

int main(int argc, char *argv[])
{
    int ret;
    float value = 0;
    struct ldal_device *device;

    printf("Analog Port Test Start\n");

    /* Register device */
    ret = ldal_device_register(&ai0.device, ai0.device_name, ai0.file_name, LDAL_CLASS_ANALOG, (void *)&ai0);
    if (ret != LDAL_EOK) {
        printf("Register analog device failed\n");
    }
    
    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("AI0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init analog device failed\n");
        return -1;
    }

    printf("Reading analog port ...\n");

    for (int i = 0; i < 5; i++) {
        
        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read analog device failed\n");
            goto __exit;
        }
        printf("> %f\n", value);
        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop analog device failed\n");
    }

    ldal_device_unregister(device);
    printf("Analog Port Test End\n");
    return 0;
}
