#include <stdio.h>
#include "ldal_memory.h"

static struct ldal_memory_device mem0 =
{
    "mem0",
    "/dev/null",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;

    printf("Memory Test Start\n");

    ret = memory_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register memory class failed\n");
    }

    ret = ldal_device_register(&mem0.device, mem0.device_name, mem0.file_name, LDAL_CLASS_MEMORY, &mem0);
    if (ret != LDAL_EOK) {
        printf("Register memory device failed\n");
    }

    /* Get device handler */
    device = ldal_device_get_by_name("mem0");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s\n", device->name);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init memory device failed\n");
        return -1;
    }

    char str[32] = "Hello, World!";
    write_device(device, str, strlen(str));

    char buf[40] = {0};
    read_device(device, buf, 40);
    for (int i=0; i<40; i++) {
        printf("%0x ", buf[i]);
    }
    printf("\n");
    printf("Read: %s\n", buf);
    
    stop_device(device);

    printf("Memory Test End\n");
    return 0;
}
