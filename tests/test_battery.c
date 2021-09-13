#include <stdio.h>
#include "ldal.h"

static struct ldal_device_table device_table[] = {
    { "battery", "/dev/k37xdev_battery", LDAL_CLASS_MISC },
};

int main(int argc, char *argv[])
{
    int ret;
    int value = 0;
    char supply_state = 0;
    char charge_state = 0;
    struct ldal_device *device;

    printf("Battery Test Start\n");

    /* Register device */
    ldal_device_create(device_table, ARRAY_SIZE(device_table));

    ldal_show_device_list();

    /* Get device handler */
    device = ldal_device_get_by_name("battery");
    if (device == NULL) {
        printf("Can't get device\n");
    }
    printf("Device: %s -> %s\n", device->name, device->filename);

    ret = startup_device(device);
    if (ret != LDAL_EOK) {
        printf("Init misc device failed\n");
        return -1;
    }

    printf("Reading battery power ...\n");
    for (int i=0; i<5; i++) {

        ret = read_device(device, &value, sizeof(value));
        if (ret != LDAL_EOK) {
            printf("Read misc device failed\n");
            goto __exit;
        }

        ret = control_device(device, BATTERY_GET_POWER_SUPPLY, &supply_state);
        if (ret != LDAL_EOK) {
            printf("Get battery supply state failed\n");
            goto __exit;
        }

        ret = control_device(device, BATTERY_GET_POWER_CHARGE, &charge_state);
        if (ret != LDAL_EOK) {
            printf("Get battery charge state failed\n");
            goto __exit;
        }

        //printf("> 0x%02x, flag: 0x%02x, power: %d %%\n", value, value >> 6, value & 0x3f);

        printf("--------------------\n");
        printf("Power: %d%%\n", value);
        printf("Supply State: %x\n", supply_state);
        printf("Charge State: %x\n", charge_state);

        sleep(1);
    }

__exit:
    ret = stop_device(device);
    if (ret != LDAL_EOK)
    {
        printf("Stop misc device failed\n");
    }

    printf("Battery Test End\n");
    return 0;
}
