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

    /* Get device handler */
    device = ldal_device_get_by_name("serial0");
    if (device != NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s\n", device->name);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init serial device failed\n");
        return -1;
    }

    char buf[40] = {0};
    read_device(device, buf, 40);
    for (int i=0; i<40; i++) {
        printf("%0x ", buf[i]);
    }
    printf("\n");
    
    ret = stop_device(device);
    if (ret != LDAL_EOK) {
        printf("Stop serial device failed\n");
    }

    printf("Serial Port Test End\n");
    return 0;
}
