#include <stdio.h>
#include <stdlib.h>
#include "ldal.h"

static char name[32];

static struct ldal_device_table device_table[] = {
    { "DI0", "/dev/didev0", LDAL_CLASS_DIGITAL },
    { "DI1", "/dev/didev1", LDAL_CLASS_DIGITAL },
    { "DI2", "/dev/didev2", LDAL_CLASS_DIGITAL },
    { "DI3", "/dev/didev3", LDAL_CLASS_DIGITAL },
    { "DI4", "/dev/didev4", LDAL_CLASS_DIGITAL },
    { "DI5", "/dev/didev5", LDAL_CLASS_DIGITAL },
    { "DI6", "/dev/didev6", LDAL_CLASS_DIGITAL },
    { "DI7", "/dev/didev7", LDAL_CLASS_DIGITAL },
};

int main(int argc, char *argv[])
{
    int ret, num = 0, value = 0;
    struct ldal_device *device;

    if (argc > 1) {
        num = atoi(argv[1]);
    }
    snprintf(name, 32, "DI%d", num);

    printf("Digital Port Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name(name);
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
