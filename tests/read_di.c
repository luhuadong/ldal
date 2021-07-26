#include <stdio.h>
#include <stdlib.h>
#include "ldal.h"

static char name[32];
static char file_name[32];

static struct ldal_digital_device di = { &name, &file_name };

int main(int argc, char *argv[])
{
    int ret, num = 0, value = 0;
    struct ldal_device *device;

    if (argc > 1) {
        num = atoi(argv[1]);
    }

    snprintf(name, 32, "DI%d", num);
    snprintf(file_name, 32, "/dev/didev%d", num);

    printf("Digital Port Test Start\n");

    /* Register device */
    ret = ldal_device_register(&di.device, di.device_name, di.file_name, LDAL_CLASS_DIGITAL, (void *)&di);
    if (ret != LDAL_EOK) {
        printf("Register digital device failed\n");
    }

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
