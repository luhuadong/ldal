#include <stdio.h>
#include "ldal_backlight.h"

static int get_from_file(const char *path, int *value)
{
    assert(path);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return -LDAL_ERROR;
    }

    if (EOF == fscanf(fp, "%d", value)) {
        fclose(fp);
        return -LDAL_ERROR;
    }

    fclose(fp);
    return LDAL_EOK;
}

static int set_to_file(const char *path, int value)
{
    assert(path);

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        return -LDAL_ERROR;
    }

    if (0 > fprintf(fp, "%d", value)) {
        fclose(fp);
        return -LDAL_ERROR;
    }

    fclose(fp);
    return LDAL_EOK;
}

static int backlight_init(struct ldal_device *dev)
{
    assert(dev);

    int value;
    char path[LDAL_FILENAME_MAX * 2] = {0};
    struct ldal_backlight_device *bl = (struct ldal_backlight_device *)dev->user_data;

    snprintf(path, sizeof(path), "%s/max_brightness", dev->filename);

    if (LDAL_EOK == get_from_file(path, &value)) {
        bl->max = value;
        if (value > 32) {
            bl->min = value / 5;  /* The limit min value is 5% of the max */
        } else {
            bl->min = 1;
        }
    } else {
        bl->min = SYS_BACKLIGHT_MIN;
        bl->max = SYS_BACKLIGHT_MAX;
    }

    return LDAL_EOK;
}

static int backlight_open(struct ldal_device *dev)
{
    assert(dev);

    char path[LDAL_FILENAME_MAX * 2] = {0};

    snprintf(path, sizeof(path), "%s/brightness", dev->filename);
    if (-1 == access(path, F_OK)) {
        return -LDAL_ERROR;
    }

    return LDAL_EOK;
}

static int backlight_close(struct ldal_device *dev)
{
    assert(dev);

    return LDAL_EOK;
}

static int backlight_read(struct ldal_device *dev, void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    char path[LDAL_FILENAME_MAX * 2] = {0};
    int *value = (int *)buf;
    int sys_val;

    struct ldal_backlight_device *bl = (struct ldal_backlight_device *)dev->user_data;

    if (len < sizeof(int)) {
        return -LDAL_EINVAL;
    }

    snprintf(path, sizeof(path), "%s/brightness", dev->filename);
    if (LDAL_EOK != get_from_file(path, &sys_val)) {
        return -LDAL_ERROR;
    }

    float delta = (float)(bl->max - bl->min) / USR_BACKLIGHT_MAX;
    *value = (sys_val - bl->min) / delta;
    
    return LDAL_EOK;
}

static int backlight_write(struct ldal_device *dev, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);

    char path[LDAL_FILENAME_MAX * 2] = {0};
    int *value = (int *)buf;

    struct ldal_backlight_device *bl = (struct ldal_backlight_device *)dev->user_data;

    if (len < sizeof(int)) {
        return -LDAL_EINVAL;
    }

    if (*value < USR_BACKLIGHT_MIN || *value > USR_BACKLIGHT_MAX) {
        return -LDAL_EINVAL;
    }

    float delta = (float)(bl->max - bl->min) / USR_BACKLIGHT_MAX;
    int sys_val = (*value) * delta + bl->min;

    snprintf(path, sizeof(path), "%s/brightness", dev->filename);

    return set_to_file(path, sys_val);
}

const struct ldal_device_ops backlight_device_ops = 
{
    .init  = backlight_init,
    .open  = backlight_open,
    .close = backlight_close,
    .read  = backlight_read,
    .write = backlight_write,
};

int backlight_device_class_register(void)
{
    struct ldal_device_class *class = NULL;

    class = (struct ldal_device_class *) calloc(1, sizeof(struct ldal_device_class));
    if (class == NULL)
    {
        perror("no memory for backlight device class create.");
        return -LDAL_ENOMEM;
    }

    class->class_id = LDAL_CLASS_BACKLIGHT;
    class->device_ops = &backlight_device_ops;

    printf("Register backlight device successfully\n");

    return ldal_device_class_register(class, LDAL_CLASS_BACKLIGHT);
}
INIT_CLASS_EXPORT(backlight_device_class_register);
