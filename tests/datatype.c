#include <stdio.h>
#include "ldal.h"


int main(void)
{
    printf("Size of LDAL Device \t= %lu\n", sizeof(struct ldal_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_MEMORY], sizeof(struct ldal_memory_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_FILE], sizeof(struct ldal_file_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_SERIAL], sizeof(struct ldal_serial_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_GPIO], sizeof(struct ldal_gpio_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_DIGITAL], sizeof(struct ldal_digital_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_ANALOG], sizeof(struct ldal_analog_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_RTC], sizeof(struct ldal_rtc_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_UDP], sizeof(struct ldal_udp_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_TCP], sizeof(struct ldal_tcp_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_ME], sizeof(struct ldal_me_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_MISC], sizeof(struct ldal_misc_device));
    printf("Size of %s \t= %lu\n", class_label[LDAL_CLASS_BACKLIGHT], sizeof(struct ldal_backlight_device));
    return 0;
}