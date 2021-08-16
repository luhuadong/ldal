#include <stdio.h>
#include "ldal.h"

#define IBUTTON_CODE_SIZE    8

static struct ldal_misc_device ibtn = {
    "ibutton",
    "/dev/k37xdev_ibutton",
};

int main(int argc, char *argv[])
{
    int ret;
    char data[IBUTTON_CODE_SIZE];
    struct ldal_device *device;

    establish_serve();
    printf("iButton Test Start\n");

    /* Register device */
    ret = ldal_device_register(&ibtn.device, ibtn.device_name, ibtn.file_name, LDAL_CLASS_MISC, (void *)&ibtn);
    if (ret != LDAL_EOK) {
        printf("Register misc device failed\n");
    }

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("ibutton");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init misc device failed\n");
        return -1;
    }

    printf("Read ibutton ...\n");
    for (int i = 0; i < 10; i++) {

        sleep(1);
        memset(data, 0, sizeof(data));

        ret = read_device(device, &data, sizeof(data));
        if (ret != LDAL_EOK) {
            printf("Read misc device failed\n");
            continue;
        }
        printf("[%d] >", i);
        for (int j=0; j<sizeof(data); j++) {
            printf(" %02x", data[j]);
        }
        printf("\n");
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop misc device failed\n");
    }

    printf("iButton Test End\n");
    return 0;
}

