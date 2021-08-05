#include <stdio.h>
#include <assert.h>

#define LDAL_GLOBALS
#include "ldal.h"

char *class_label[LDAL_CLASS_MAX] = {
    "Memory Device",
    "File Device",
    "Serial Device",
    "GPIO Device",
    "Digital I/O",
    "Analog Input",
    "RTC Device",
    "UDP Socket",
    "TCP Socket",
    "Mobile Equipment",
    "Misc Device",
    "Backlight Device"
};

/* The global list: class list & device list */
static struct list_head ldal_device_class_list = LIST_HEAD_INIT(ldal_device_class_list);
static struct list_head ldal_device_list = LIST_HEAD_INIT(ldal_device_list);

static pthread_rwlock_t rwlock_cls = PTHREAD_RWLOCK_INITIALIZER;
static pthread_rwlock_t rwlock_dev = PTHREAD_RWLOCK_INITIALIZER;

static uint16_t g_device_num = 0;

struct list_head *ldal_get_device_list(void)
{
    return &ldal_device_list;
}

int startup_device(ldal_device_t * const device)
{
    int ret = LDAL_EOK;

    if (false == device->is_init) {
        return -LDAL_ERROR;
    }

    if (device->ref > 0) {
        device->ref += 1;
        return LDAL_EOK;
    }

    ret = device->class->device_ops->open(device);
    if (ret == LDAL_EOK) {
        device->ref += 1;
    }
    return LDAL_EOK;
}

int stop_device(ldal_device_t * const device)
{
    int ret = LDAL_EOK;

    if (false == device->is_init) {
        return -LDAL_ERROR;
    }

    if (device->ref > 0) {
        device->ref -= 1;

        if (device->ref == 0) {
            ret = device->class->device_ops->close(device);
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
    int ret;
    pthread_mutex_lock(&device->mutex);
    ret = device->class->device_ops->control(device, cmd, arg);
    pthread_mutex_unlock(&device->mutex);
    return ret;
}

int bind_local_addr(struct ldal_device *device, const char *ipaddr, const uint16_t port)
{
    int ret;
    pthread_mutex_lock(&device->mutex);
    ret = device->class->device_ops->bind(device, ipaddr, port);
    pthread_mutex_unlock(&device->mutex);
    return ret;
}

int connect_server_addr(struct ldal_device *device, const char *ipaddr, const uint16_t port)
{
    int ret;
    pthread_mutex_lock(&device->mutex);
    ret = device->class->device_ops->connect(device, ipaddr, port);
    pthread_mutex_unlock(&device->mutex);
    return ret;
}

void ldal_show_device_list(void)
{
    struct ldal_device *device = NULL;

    pthread_rwlock_rdlock(&rwlock_dev);

    list_for_each_entry(device, &ldal_device_list, list)
    {
        printf("!_ %s: %s -> %s\n", class_label[device->class->class_id], device->name, device->filename);
    }

    pthread_rwlock_unlock(&rwlock_dev);
}
void ldal_show_device_list (void) __attribute__ ((destructor));

/**
 * This function will get LDAL device by device name.
 *
 * @param type the name type
 * @param name the device name of the client name
 *
 * @return the LDAL device structure pointer
 */
struct ldal_device *ldal_device_get_by_name(const char *name)
{
    assert(name);
    struct ldal_device *device = NULL;

    pthread_rwlock_rdlock(&rwlock_dev);

    list_for_each_entry(device, &ldal_device_list, list)
    {
        if (strncmp(device->name, name, strlen(name)) == 0) {
            pthread_rwlock_unlock(&rwlock_dev);
            return device;
        }
    }
    pthread_rwlock_unlock(&rwlock_dev);
    return NULL;
}

struct ldal_device *ldal_device_get_by_type(const int type, const char *name)
{
    assert(name);
    struct ldal_device *device = NULL;

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

struct ldal_device *ldal_device_get_object_by_name(const char *name)
{
    assert(name);
    struct ldal_device *device = NULL;

    device = ldal_device_get_by_name(name);
    if (device) {
        return device->user_data;
    }
    return NULL;
}


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
    INIT_LIST_HEAD(&class->list);

    /* Add current device class to list */
    pthread_rwlock_wrlock(&rwlock_cls);
    list_add_tail(&(class->list), &ldal_device_class_list);
    pthread_rwlock_unlock(&rwlock_cls);

    return LDAL_EOK;
}

/* Get AT device class by client ID */
static struct ldal_device_class *ldal_device_class_get(ldal_class_t class_id)
{
    //struct list_head *node = NULL;
    struct ldal_device_class *class = NULL;

    pthread_rwlock_rdlock(&rwlock_cls);

    /* Get device class by class ID */
    list_for_each_entry(class, &ldal_device_class_list, list)
    {
        if (class->class_id == class_id) {
            pthread_rwlock_unlock(&rwlock_cls);
            return class;
        }
    }
    pthread_rwlock_unlock(&rwlock_cls);
    return NULL;
}

static void ldal_device_class_set(ldal_class_t class_id)
{
    switch (class_id)
    {
    case LDAL_CLASS_MEMORY:
    {
        memory_device_class_register();
        break;
    }
    case LDAL_CLASS_FILE:
    {
        file_device_class_register();
        break;
    }
    case LDAL_CLASS_SERIAL:
    {
        serial_device_class_register();
        break;
    }
    case LDAL_CLASS_GPIO:
    {
        gpio_device_class_register();
        break;
    }
    case LDAL_CLASS_DIGITAL:
    {
        digital_device_class_register();
        break;
    }
    case LDAL_CLASS_ANALOG:
    {
        analog_device_class_register();
        break;
    }
    case LDAL_CLASS_RTC:
    {
        rtc_device_class_register();
        break;
    }
    case LDAL_CLASS_UDP:
    {
        udp_device_class_register();
        break;
    }
    case LDAL_CLASS_TCP:
    {
        tcp_device_class_register();
        break;
    }
    case LDAL_CLASS_ME:
    {
        me_device_class_register();
        break;
    }
    case LDAL_CLASS_MISC:
    {
        misc_device_class_register();
        break;
    }
    default:
        break;
    }
}

/**
 * This function registers an device with specified device name and file name.
 *
 * @param device the pointer of ldal device structure
 * @param device_name device name
 * @param file_name device file name
 * @param class_id device class ID
 * @param user_data user-specific data, pointer to parent structure
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

    if (NULL != ldal_device_get_by_name(device_name)) {
        printf("Device name '%s' has been registered.\n", device_name);
        result = -LDAL_EINVAL;
        goto __exit;
    }

    class = ldal_device_class_get(class_id);
    if (class == NULL) {
        ldal_device_class_set(class_id);
        class = ldal_device_class_get(class_id);
        if (class == NULL) {
            printf("Get %s class failed.\n", class_label[class_id]);
            result = -LDAL_ERROR;
            goto __exit;
        }
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
    pthread_rwlock_wrlock(&rwlock_dev);

    /* Add current device to device list */
    list_add_tail(&(device->list), &ldal_device_list);
    g_device_num += 1;

    /* unlock */
    pthread_rwlock_unlock(&rwlock_dev);

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

int ldal_device_unregister(struct ldal_device *device)
{
    if (device == NULL)
        return -LDAL_EINVAL;

    if (device->ref > 0)
        return -LDAL_ERROR;
    
    pthread_rwlock_wrlock(&rwlock_dev);
    list_del(&(device->list));
    g_device_num -= 1;
    pthread_rwlock_unlock(&rwlock_dev);
    
    return LDAL_EOK;
}

static ldal_device_t *_device_create(struct ldal_device_table *table)
{
    ldal_device_t *dev = NULL;

    const char *device_name = table->device_name;
    const char *file_name = table->file_name;
    ldal_class_t class_id = table->class_id;

    switch (class_id)
    {
    case LDAL_CLASS_MEMORY: 
    {
        struct ldal_memory_device *object;
        object = (struct ldal_memory_device *)calloc(1, sizeof(struct ldal_memory_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_FILE: 
    {
        struct ldal_file_device *object;
        object = (struct ldal_file_device *)calloc(1, sizeof(struct ldal_file_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_SERIAL: 
    {
        struct ldal_serial_device *object;
        object = (struct ldal_serial_device *)calloc(1, sizeof(struct ldal_serial_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_GPIO: 
    {
        struct ldal_gpio_device *object;
        object = (struct ldal_gpio_device *)calloc(1, sizeof(struct ldal_gpio_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_DIGITAL: 
    {
        struct ldal_digital_device *object;
        object = (struct ldal_digital_device *)calloc(1, sizeof(struct ldal_digital_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_ANALOG:
    {
        struct ldal_analog_device *object;
        object = (struct ldal_analog_device *)calloc(1, sizeof(struct ldal_analog_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        object->pos = (int)table->private_data;
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_RTC:
    {
        struct ldal_rtc_device *object;
        object = (struct ldal_rtc_device *)calloc(1, sizeof(struct ldal_rtc_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_UDP: 
    {
        struct ldal_udp_device *object;
        object = (struct ldal_udp_device *)calloc(1, sizeof(struct ldal_udp_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_TCP: 
    {
        struct ldal_tcp_device *object;
        object = (struct ldal_tcp_device *)calloc(1, sizeof(struct ldal_tcp_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_ME: 
    {
        struct ldal_me_device *object;
        object = (struct ldal_me_device *)calloc(1, sizeof(struct ldal_me_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_MISC: 
    {
        struct ldal_misc_device *object;
        object = (struct ldal_misc_device *)calloc(1, sizeof(struct ldal_misc_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    case LDAL_CLASS_BACKLIGHT: 
    {
        struct ldal_backlight_device *object;
        object = (struct ldal_backlight_device *)calloc(1, sizeof(struct ldal_backlight_device));
        if (object == NULL) {
            perror("calloc on create");
            break;
        }
        ldal_device_register(&object->device, device_name, file_name, class_id, (void *)object);
        dev = &object->device;
        break;
    }
    default: break;
    }

    return dev;
}

int ldal_device_create(struct ldal_device_table *table, const size_t size)
{
    int ret = LDAL_EOK;

    assert(table);
    if (0 == size)
        return -LDAL_EINVAL;

    for (int i=0; i<size; i++) {

        if (NULL == _device_create(&table[i])) {
            ret = -LDAL_ENOMEM;
            goto __create_exit;
        }
    }

__create_exit:

    return ret;
}
