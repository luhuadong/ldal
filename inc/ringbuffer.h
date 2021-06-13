/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#ifndef __DG_RINGBUFFER_H__
#define __DG_RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define RT_ALIGN_SIZE 4

/* ring buffer */
struct dg_ringbuffer
{
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    uint16_t read_mirror : 1;
    uint16_t read_index : 15;
    uint16_t write_mirror : 1;
    uint16_t write_index : 15;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    int16_t buffer_size;
};

enum dg_ringbuffer_state
{
    DG_RINGBUFFER_EMPTY,
    DG_RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    DG_RINGBUFFER_HALFFULL,
};

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void dg_ringbuffer_init(struct dg_ringbuffer *rb, uint8_t *pool, int16_t size);
void dg_ringbuffer_reset(struct dg_ringbuffer *rb);
size_t dg_ringbuffer_put(struct dg_ringbuffer *rb, const uint8_t *ptr, uint16_t length);
size_t dg_ringbuffer_put_force(struct dg_ringbuffer *rb, const uint8_t *ptr, uint16_t length);
size_t dg_ringbuffer_putchar(struct dg_ringbuffer *rb, const uint8_t ch);
size_t dg_ringbuffer_putchar_force(struct dg_ringbuffer *rb, const uint8_t ch);
size_t dg_ringbuffer_get(struct dg_ringbuffer *rb, uint8_t *ptr, uint16_t length);
size_t dg_ringbuffer_peak(struct dg_ringbuffer *rb, uint8_t **ptr);
size_t dg_ringbuffer_getchar(struct dg_ringbuffer *rb, uint8_t *ch);
size_t dg_ringbuffer_data_len(struct dg_ringbuffer *rb);

#ifdef RT_USING_HEAP
struct dg_ringbuffer* dg_ringbuffer_create(uint16_t length);
void dg_ringbuffer_destroy(struct dg_ringbuffer *rb);
#endif

inline uint16_t dg_ringbuffer_get_size(struct dg_ringbuffer *rb)
{
    assert(rb != NULL);
    return rb->buffer_size;
}

/** return the size of empty space in rb */
#define dg_ringbuffer_space_len(rb) ((rb)->buffer_size - dg_ringbuffer_data_len(rb))


#ifdef __cplusplus
}
#endif

#endif /* __DG_RINGBUFFER_H__ */
