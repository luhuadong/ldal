#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

#include "ldal_memory.h"
#include "ringbuffer.h"

#define DEFAULT_MEM_SIZE 4096    /* Also can use getpagesize() */

static int memory_open(struct ldal_device *device)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;

    struct dg_ringbuffer *rb = dg_ringbuffer_create(DEFAULT_MEM_SIZE);
    if (!rb) {
        printf("Alloc memory for ringbuffer failed\n");
        return -LDAL_ENOMEM;
    }

    mem->user_data = (void *)rb;
    return LDAL_EOK;
}

static int memory_close(struct ldal_device *device)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;

    if (mem->user_data) {
        dg_ringbuffer_destroy((struct dg_ringbuffer *)mem->user_data);
    }

    return LDAL_EOK;
}

static int memory_read(struct ldal_device *device, void *buf, size_t len)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;
    struct dg_ringbuffer *rb = mem->user_data;

    if (!rb) {
        return -LDAL_ERROR;
    }

    size_t size = dg_ringbuffer_get(rb, buf, len);
    return size;
}

static int memory_write(struct ldal_device *device, const void *buf, size_t len)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;
    struct dg_ringbuffer *rb = mem->user_data;

    if (!rb) {
        return -LDAL_ERROR;
    }

    size_t size = dg_ringbuffer_put(rb, buf, len);
    return size;
}

/* reset memory */
static int memory_control(struct ldal_device *device, int cmd, void *arg)
{
    return LDAL_EOK;
}

const struct ldal_device_ops memory_device_ops = {

    .open  = memory_open,
    .close = memory_close,
    .read  = memory_read,
    .write = memory_write,
    .control = memory_control,
};

int memory_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *)calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for memory device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_MEMORY;
    class->device_ops = &memory_device_ops;

    debug_print("Register memory class successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_MEMORY);
}
INIT_CLASS_EXPORT(memory_device_class_register);
