#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "ldal.h"


/* The global list of device class */
static struct list_head ldal_device_class_list = LIST_HEAD_INIT(ldal_device_class_list);

/* The global list of device */
static struct list_head ldal_device_list = LIST_HEAD_INIT(ldal_device_list);

int startup_device(const char *dev_name)
{
    struct ldal_device *device = NULL;
    device = ldal_device_get_by_name(dev_name);
    if (device == NULL) {
        return -LDAL_ERROR;
    }

    return device->class->device_ops->open(device);
}

int stop_device(const char *dev_name)
{
    struct ldal_device *device = NULL;
    device = ldal_device_get_by_name(dev_name);
    if (device == NULL) {
        return -LDAL_ERROR;
    }

    return device->class->device_ops->close(device);
}

int read_device(const char *dev_name, char *buff, int len)
{
    struct ldal_device *device = NULL;
    device = ldal_device_get_by_name(dev_name);
    if (device == NULL) {
        return -LDAL_ERROR;
    }

    return device->class->device_ops->read(device, buff, len);
}

int write_device(const char *dev_name, char *buff, int len)
{
    struct ldal_device *device = NULL;
    device = ldal_device_get_by_name(dev_name);
    if (device == NULL) {
        return -LDAL_ERROR;
    }

    return device->class->device_ops->write(device, buff, len);
}

int control_device(const char *dev_name, int cmd, void *arg)
{
    return LDAL_EOK;
}

int config_device(const char *dev_name, int cmd, void *arg)
{
    return LDAL_EOK;
}

int read_device_ai_src_value(const char *dev_name, float *value)
{
    return LDAL_EOK;
}

/**
 * This function will get LDAL device by device name.
 *
 * @param type the name type
 * @param name the device name or the client name
 *
 * @return the LDAL device structure pointer
 */
#if 1
struct ldal_device *ldal_device_get_by_name(const char *name)
{
    struct ldal_device *device = NULL;

    assert(name);

    list_for_each_entry(device, &ldal_device_list, list)
    {
        if (strncmp(device->name, name, strlen(name)) == 0) {
                return device;
        }
    }
    return NULL;
}
#else
struct ldal_device *ldal_device_get_by_name(int type, const char *name)
{
    //struct list_head *node = NULL;
    struct ldal_device *device = NULL;

    assert(name);

    list_for_each_entry(device, &ldal_device_list, list)
    {
        if (device->class->class_id == type) {
            if (strncmp(device->name, name, strlen(name)) == 0) {
                return device;
            }
            return device;
        }
    }
    return NULL;
}
#endif

/**
 * This function registers an AT device class with specified device class ID.
 *
 * @param class the pointer of AT device class structure
 * @param class_id AT device class ID
 *
 * @return 0: register successfully
 */
int ldal_device_class_register(struct ldal_device_class *class, uint16_t class_id)
{
    assert(class);

    /* Fill device class */
    class->class_id = class_id;

    /* Initialize current device class single list */
    //class->list = LIST_HEAD_INIT(class->list);
    INIT_LIST_HEAD(&class->list);

    /* Add current device class to list */
    list_add_tail(&(class->list), &ldal_device_class_list);

    return LDAL_EOK;
}

/* Get AT device class by client ID */
static struct ldal_device_class *ldal_device_class_get(uint16_t class_id)
{
    //struct list_head *node = NULL;
    struct ldal_device_class *class = NULL;

    /* Get AT device class by class ID */
    list_for_each_entry(class, &ldal_device_class_list, list)
    {
        if (class->class_id == class_id) {
            return class;
        }
    }
    return NULL;
}

/**
 * This function registers an AT device with specified device name and AT client name.
 *
 * @param device the pointer of AT device structure
 * @param device_name AT device name
 * @param at_client_name AT device client name
 * @param class_id AT device class ID
 * @param user_data user-specific data
 *
 * @return = 0: register successfully
 *         < 0: register failed
 */
int ldal_device_register(struct ldal_device *device, const char *device_name, 
                         const char *file_name, uint16_t class_id, void *user_data)
{
    int result = 0;
    struct ldal_device_class *class = NULL;


    assert(device);
    assert(device_name);
    assert(file_name);

    class = ldal_device_class_get(class_id);
    if (class == NULL)
    {
        printf("get AT device class(%d) failed.", class_id);
        result = -LDAL_ERROR;
        goto __exit;
    }

    memcpy(device->name, device_name, strlen(device_name));
    memcpy(device->filename, file_name, strlen(file_name));
    device->class = class;
    device->user_data = user_data;

    //device->list = LIST_HEAD_INIT(device->list);
    INIT_LIST_HEAD(&device->list);

    /* lock */

    /* Add current device to device list */
    list_add_tail(&(device->list), &ldal_device_list);

    /* unlock */

__exit:
    if (result < 0)
        device->is_init = false;
    else
        device->is_init = true;

    return LDAL_EOK;
}
