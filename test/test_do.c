#include <stdio.h>
#include "ldal.h"

static struct ldal_digital_device do0 = {
    "DO1",
    "/dev/dodev1",
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;

    printf("Digital Port Test Start\n");

    /* Register device */
    ret = ldal_device_register(&do0.device, do0.device_name, do0.file_name, LDAL_CLASS_DIGITAL, (void *)&do0);
    if (ret != LDAL_EOK) {
        printf("Register digital device failed\n");
    }
    
    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("DO1");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init digital device failed\n");
        return -1;
    }

    printf("Writing digital port ...\n");

    for (int i = 0; i < 5; i++) {

        value = !value;
        ret = write_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Write digital device failed\n");
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
