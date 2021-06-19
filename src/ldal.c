#include <stdio.h>
#include <assert.h>

#include "ldal.h"

static char *device_label[] = {
    "memory",
    "file",
    "serial",
    "gpio",
    "digital",
    "analog",
    "rtc",
    "socket"
};


/* The global list of device class */
static struct list_head ldal_device_class_list = LIST_HEAD_INIT(ldal_device_class_list);

/* The global list of device */
static struct list_head ldal_device_list = LIST_HEAD_INIT(ldal_device_list);

int startup_device(ldal_device_t * const device)
{
    int ret;

    if (false == device->is_init) {
        return LDAL_ERROR;
    }

    if (device->ref > 0) {
        device->ref += 1;
        return LDAL_EOK;
    }

    ret = device->class->device_ops->open(device);
    if (ret == LDAL_EOK) {
        device->ref += 1;
    }
    return ret;
}

int stop_device(ldal_device_t * const device)
{
    int ret = LDAL_EOK;

    if (false == device->is_init) {
        return LDAL_ERROR;
    }

    if (device->ref > 0)
    {
        device->ref -= 1;

        if (device->ref == 0)
        {
            ret = device->class->device_ops->close(device);
            if (ret == LDAL_EOK) {
                pthread_mutex_destroy(&device->mutex);
                device->is_init = false;
            }
        }
    }
    return ret;
}

int read_device(ldal_device_t * const device, void *buff, const size_t len)
{
    int ret;
    pthread_mutex_lock(&device->mutex);
    ret = device->class->device_ops->read(device, buff, len);
    pthread_mutex_unlock(&device->mutex);
    return ret;
}

int write_device(ldal_device_t * const device, const void *buff, const size_t len)
{
    int ret;
    pthread_mutex_lock(&device->mutex);
    ret = device->class->device_ops->write(device, buff, len);
    pthread_mutex_unlock(&device->mutex);
    return ret;
}

int control_device(ldal_device_t * const device, int cmd, void *arg)
{
    return LDAL_EOK;
}

int config_device(ldal_device_t * const device, int cmd, void *arg)
{
    return LDAL_EOK;
}

int read_device_ai_src_value(ldal_device_t * const device, float *value)
{
    return LDAL_EOK;
}

void ldal_show_device_list(void)
{
    struct ldal_device *device = NULL;

    list_for_each_entry(device, &ldal_device_list, list)
    {
        printf("!_ %s device: %s -> %s\n", device_label[device->class->class_id], device->name, device->filename);
    }
}
void ldal_show_device_list (void) __attribute__ ((destructor));

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
int ldal_device_class_register(struct ldal_device_class *class, ldal_class_t class_id)
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
static struct ldal_device_class *ldal_device_class_get(ldal_class_t class_id)
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
                         const char *file_name, ldal_class_t class_id, void *user_data)
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
    pthread_mutex_init(&device->mutex, NULL);
    device->ref = 0;

    //device->list = LIST_HEAD_INIT(device->list);
    INIT_LIST_HEAD(&device->list);

    /* lock */

    /* Add current device to device list */
    list_add_tail(&(device->list), &ldal_device_list);

    /* unlock */

    /* Finally initialize device if it has init function */
    if (device->class->device_ops->init) {
        device->class->device_ops->init(device);
    }

__exit:
    if (result < 0)
        device->is_init = false;
    else
        device->is_init = true;

    return LDAL_EOK;
}
