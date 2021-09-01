#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table device_table[] = {
    { "file0", "/tmp/hello.txt", LDAL_CLASS_FILE },
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    char buf[1024] = {0};

    establish_serve();
    printf("FILE Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));
    
    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("file0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init file device failed\n");
        return -1;
    }

    char *str = "hello, world! superman\n";
    printf("Writing file ...\n");

    ret = write_device(device, str, strlen(str));
    if (ret != LDAL_EOK) {
        printf("Write file device failed\n");
        goto __exit;
    }

    sleep(1);

    printf("Reading file ...\n");

    ret = read_device(device, buf, 13);
    if (ret != LDAL_EOK) {
        printf("Read file device failed\n");
        goto __exit;
    }
    printf("> %s\n", buf);

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK) {
        printf("Stop file device failed\n");
    }

    printf("File Test End\n");
    return 0;
}
