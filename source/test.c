#include <stdio.h>
#include "dev_serial.h"

static struct ldal_serial_device serial0 =
{
    "serial0",
    "/dev/ttyUSB0",
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;

    printf("Serial Port Test Start\n");

    ret = serial_device_class_register();
    if (ret != LDAL_EOK) {
        printf("Register serial class failed\n");
    }

    ret = ldal_device_register(&serial0.device, serial0.device_name, serial0.file_name, LDAL_CLASS_SERIAL, (void *) &serial0);
    if (ret != LDAL_EOK) {
        printf("Register serial device failed\n");
    }

    device = ldal_device_get_by_name("serial0");
    printf("Device: %s\n", device->name);

    ret = startup_device("serial0");
    if (ret != LDAL_EOK) {
        printf("Init serial device failed\n");
    }
    
    ret = stop_device("serial0");
    if (ret != LDAL_EOK) {
        printf("Init serial device failed\n");
    }

    printf("Serial Port Test End\n");
    return 0;
}
