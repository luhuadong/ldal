#include <stdio.h>
#include <signal.h>

#include "ldal.h"

/* The global list of device */
static rt_slist_t ldal_device_list = RT_SLIST_OBJECT_INIT(at_device_list);

/* The global list of device class */
static rt_slist_t ldal_device_class_list = RT_SLIST_OBJECT_INIT(at_device_class_list);

int ldal_device_class_register(struct ldal_device_class *class, uint16_t class_id)
{

}

int ldal_device_register(struct ldal_device *device, const char *devname, const char *filename, uint16_t class_id, void *user_data)
{

}