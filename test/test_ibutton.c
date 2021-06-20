#include <stdio.h>
#include "ldal_misc.h"

static struct ldal_misc_device ibtn = {
    "ibutton",
    "/dev/k37adev_ibutton",
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;

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

    printf("Toggle ibutton ...\n");
    for (int i = 0; i < 5; i++) {

        value = !value;
        ret = write_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Write misc device failed\n");
            goto __exit;
        }
        sleep(1);

        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK)
        {
            printf("Read misc device failed\n");
            goto __exit;
        }
        printf("> %d\n", value);
        sleep(1);
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

