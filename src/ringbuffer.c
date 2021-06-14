#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuffer.h"

static enum dg_ringbuffer_state dg_ringbuffer_status(struct dg_ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            return DG_RINGBUFFER_EMPTY;
        else
            return DG_RINGBUFFER_FULL;
    }
    return DG_RINGBUFFER_HALFFULL;
}

void dg_ringbuffer_init(struct dg_ringbuffer *rb, char *pool, int16_t size)
{
    assert(rb != NULL);
    assert(size > 0);

    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = RT_ALIGN_DOWN(size, RT_ALIGN_SIZE);
}

/**
 * put a block of data into ring buffer
 */
size_t dg_ringbuffer_put(struct dg_ringbuffer *rb, const char *ptr, uint16_t length)
{
    uint16_t size;

    assert(rb != NULL);

    /* whether has enough space */
    size = dg_ringbuffer_space_len(rb);

    /* no space */
    if (size == 0)
        return 0;

    /* drop some data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index],
           &ptr[0],
           rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0],
           &ptr[rb->buffer_size - rb->write_index],
           length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

/**
 * put a block of data into ring buffer
 *
 * When the buffer is full, it will discard the old data.
 */
size_t dg_ringbuffer_put_force(struct dg_ringbuffer *rb, const char *ptr, uint16_t length)
{
    uint16_t space_length;

    assert(rb != NULL);

    space_length = dg_ringbuffer_space_len(rb);

    if (length > rb->buffer_size)
    {
        ptr = &ptr[length - rb->buffer_size];
        length = rb->buffer_size;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;

        if (length > space_length)
            rb->read_index = rb->write_index;

        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index], &ptr[0], rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    if (length > space_length)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = rb->write_index;
    }

    return length;
}

/**
 *  get data from ring buffer
 */
size_t dg_ringbuffer_get(struct dg_ringbuffer *rb, char *ptr, uint16_t length)
{
    size_t size;

    assert(rb != NULL);

    /* whether has enough data  */
    size = dg_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length)
    {
        /* copy all of data */
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    memcpy(&ptr[0],
           &rb->buffer_ptr[rb->read_index],
           rb->buffer_size - rb->read_index);
    memcpy(&ptr[rb->buffer_size - rb->read_index],
           &rb->buffer_ptr[0],
           length - (rb->buffer_size - rb->read_index));

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

/**
 *  peak data from ring buffer
 */
size_t dg_ringbuffer_peak(struct dg_ringbuffer *rb, char **ptr)
{
    assert(rb != NULL);

    *ptr = NULL;

    /* whether has enough data  */
    size_t size = dg_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    *ptr = &rb->buffer_ptr[rb->read_index];

    if(rb->buffer_size - rb->read_index > size)
    {
        rb->read_index += size;
        return size;
    }

    size = rb->buffer_size - rb->read_index;

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = 0;

    return size;
}

/**
 * put a character into ring buffer
 */
size_t dg_ringbuffer_putchar(struct dg_ringbuffer *rb, const char ch)
{
    assert(rb != NULL);

    /* whether has enough space */
    if (!dg_ringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size-1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    }
    else
    {
        rb->write_index++;
    }

    return 1;
}

/**
 * put a character into ring buffer
 *
 * When the buffer is full, it will discard one old data.
 */
size_t dg_ringbuffer_putchar_force(struct dg_ringbuffer *rb, const char ch)
{
    enum dg_ringbuffer_state old_state;

    assert(rb != NULL);

    old_state = dg_ringbuffer_status(rb);

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size-1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
        if (old_state == DG_RINGBUFFER_FULL)
        {
            rb->read_mirror = ~rb->read_mirror;
            rb->read_index = rb->write_index;
        }
    }
    else
    {
        rb->write_index++;
        if (old_state == DG_RINGBUFFER_FULL)
            rb->read_index = rb->write_index;
    }

    return 1;
}

/**
 * get a character from a ringbuffer
 */
size_t dg_ringbuffer_getchar(struct dg_ringbuffer *rb, char *ch)
{
    assert(rb != NULL);

    /* ringbuffer is empty */
    if (!dg_ringbuffer_data_len(rb))
        return 0;

    /* put character */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size-1)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    }
    else
    {
        rb->read_index++;
    }

    return 1;
}

/**
 * get the size of data in rb
 */
size_t dg_ringbuffer_data_len(struct dg_ringbuffer *rb)
{
    switch (dg_ringbuffer_status(rb))
    {
    case DG_RINGBUFFER_EMPTY:
        return 0;
    case DG_RINGBUFFER_FULL:
        return rb->buffer_size;
    case DG_RINGBUFFER_HALFFULL:
    default:
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    };
}

/**
 * empty the rb
 */
void dg_ringbuffer_reset(struct dg_ringbuffer *rb)
{
    assert(rb != NULL);

    rb->read_mirror = 0;
    rb->read_index = 0;
    rb->write_mirror = 0;
    rb->write_index = 0;
}

struct dg_ringbuffer* dg_ringbuffer_create(uint16_t size)
{
    struct dg_ringbuffer *rb;
    char *pool;

    assert(size > 0);

    size = RT_ALIGN_DOWN(size, RT_ALIGN_SIZE);

    rb = (struct dg_ringbuffer *)malloc(sizeof(struct dg_ringbuffer));
    if (rb == NULL)
        goto exit;

    pool = (char *)malloc(size);
    if (pool == NULL)
    {
        free(rb);
        rb = NULL;
        goto exit;
    }
    bzero(pool, size);
    dg_ringbuffer_init(rb, pool, size);

exit:
    return rb;
}

void dg_ringbuffer_destroy(struct dg_ringbuffer *rb)
{
    assert(rb != NULL);

    free(rb->buffer_ptr);
    free(rb);
}
