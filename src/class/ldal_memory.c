#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

#include "ldal_memory.h"

#define DEFAULT_MEM_SIZE 4096

static int memory_open(struct ldal_device *device)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;
    int size = getpagesize();

    char *p = (char *)malloc(size);
    if (!p) {
        perror("Can't get memory");
        return -LDAL_ENOMEM;
    }
    bzero(p, size);
    mem->user_data = (void *)p;

    return LDAL_EOK;
}

static int memory_close(struct ldal_device *device)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;

    if (mem->user_data)
        free(mem->user_data);

    return LDAL_EOK;
}

static int memory_read(struct ldal_device *device, char *buf, size_t len)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;

    char *src = mem->user_data;
    if (NULL == src)
    {
        printf("Memory read invalid\n");
        return -LDAL_ERROR;
    }

    size_t size = malloc_usable_size(src);
    if (size > len)
        size = len;

    memcpy(buf, src, size);
    return size;
}

static int memory_write(struct ldal_device *device, char *buf, size_t len)
{
    assert(device);

    struct ldal_memory_device *mem = (struct ldal_memory_device *)device->user_data;

    char *dest = mem->user_data;
    if (NULL == dest)
    {
        printf("Memory write invalid\n");
        return -LDAL_ERROR;
    }

    size_t size = malloc_usable_size(dest);
    if (size > len)
        size = len;

    memcpy(dest, buf, size);
    return size;
}

/* reset memory */
static int memory_control(int fd)
{
    return LDAL_EOK;
}

const struct ldal_device_ops memory_device_ops =
    {
        .open = memory_open,
        .close = memory_close,
        .read = memory_read,
        .write = memory_write,
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

    return ldal_device_class_register(class, LDAL_CLASS_MEMORY);
}
