#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table device_table[] = {
    { "serial0", "/dev/ttyUSB0", LDAL_CLASS_SERIAL },
};

int main(int argc, char *argv[])
{
    int ret;
    struct ldal_device *device;

    printf("Serial Port Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));
    
    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("serial0");
    if (device == NULL) {
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
