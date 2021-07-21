#include <stdio.h>
#include "ldal.h"

#define ON     1
#define OFF    0

static struct ldal_device_table device_table[] = {
    { "DO0", "/dev/dodev0", LDAL_CLASS_DIGITAL },
    { "DO1", "/dev/dodev1", LDAL_CLASS_DIGITAL },
    { "DO2", "/dev/dodev2", LDAL_CLASS_DIGITAL },
    { "DO3", "/dev/dodev3", LDAL_CLASS_DIGITAL },
    { "DO4", "/dev/dodev4", LDAL_CLASS_DIGITAL },
    { "DO5", "/dev/dodev5", LDAL_CLASS_DIGITAL },
    { "DO6", "/dev/dodev6", LDAL_CLASS_DIGITAL },
    { "DO7", "/dev/dodev7", LDAL_CLASS_DIGITAL },
};

int main(int argc, char *argv[])
{
    int ret, value = 0;
    struct ldal_device *device;
    char name[8] = {0};

    printf("Digital Port Test Start\n");

    /* Register device */
    ret = ldal_device_create(&device_table, ARRAY_SIZE(device_table));
    if (ret < 0) {
        printf("Create DO devices failed\n");
        return -1;
    }
    
    ldal_show_device_list();

    printf("Turn ON digital port...\n");
    value = ON;

    for (int i=0; i<ARRAY_SIZE(device_table); i++) {

        /* Get device handler */
        sprintf(name, "DO%d", i);
        device = ldal_device_get_by_name(name);
        if (device == NULL) {
            printf("Can't get device %s\n", name);
            continue;
        }

        printf("Device: %s -> %s\n", device->name, device->filename);

        ret = startup_device(device);
        if (ret != LDAL_EOK) {
            printf("Init digital device %s failed\n", name);
            continue;
        }

        ret = write_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Write digital device %s failed\n", name);
            continue;
        }

        usleep(500 * 1000);
    }

    sleep(1);

    printf("Turn OFF digital port...\n");
    value = OFF;

    for (int i=0; i<ARRAY_SIZE(device_table); i++) {

        /* Get device handler */
        sprintf(name, "DO%d", i);
        device = ldal_device_get_by_name(name);
        if (device == NULL) {
            printf("Can't get device %s\n", name);
            continue;
        }

        printf("Device: %s -> %s\n", device->name, device->filename);

        ret = write_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Write digital device %s failed\n", name);
            continue;
        }

        ret = stop_device(device);
        if (ret != LDAL_EOK) {
            printf("Stop digital device failed\n");
        }

        usleep(500 * 1000);
    }

__exit:
    
    printf("Digital Port Test End\n");
    return 0;
}
