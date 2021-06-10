#include <stdio.h>
#include <signal.h>

#include "ldal.h"

/* The global list of device */
static struct list_head ldal_device_list = LIST_HEAD_INIT(ldal_device_list);

/* The global list of device class */
static struct list_head ldal_device_class_list = LIST_HEAD_INIT(ldal_device_class_list);

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
    //rt_base_t level;

    assert(class);

    /* Fill device class */
    class->class_id = class_id;

    /* Initialize current device class single list */
    LIST_HEAD_INIT(class->list);

    //level = rt_hw_interrupt_disable();

    /* Add current device class to list */
    list_add_tail(&(class->list), &ldal_device_class_list);

    //rt_hw_interrupt_enable(level);

    return LDAL_EOK;
}

/* Get AT device class by client ID */
static struct at_device_class *at_device_class_get(uint16_t class_id)
{
    rt_base_t level;
    rt_slist_t *node = NULL;
    struct at_device_class *class = RT_NULL;

    level = rt_hw_interrupt_disable();

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
    char name[LDAL_NAME_MAX] = {0};
    struct at_device_class *class = RT_NULL;

    assert(device);
    assert(device_name);
    assert(file_name);

    class = at_device_class_get(class_id);
    if (class == NULL)
    {
        printf("get AT device class(%d) failed.", class_id);
        result = -LDAL_ERROR;
        goto __exit;
    }

    memcpy(device->name, device_name, strlen(device_name));
    device->class = class;
    device->user_data = user_data;

    /* Initialize current AT device single list */
    LIST_HEAD_INIT(device->list);

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
