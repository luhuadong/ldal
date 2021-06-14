#include <stdio.h>
#include "ldal_file.h"

static struct ldal_file_device file0 =
{
    "file0",
    "/tmp/hello.txt",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;
    char buf[1024] = {0};

    printf("FILE Test Start\n");

    /* Register class */
    ret = file_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register file class failed\n");
    }

    /* Register device */
    ret = ldal_device_register(&file0.device, file0.device_name, file0.file_name, LDAL_CLASS_FILE, (void *) &file0);
    if (ret != LDAL_EOK) {
        printf("Register file device failed\n");
    }

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
