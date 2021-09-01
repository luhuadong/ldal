#ifndef __LDAL_PRIVATE_H__
#define __LDAL_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef LDAL_GLOBALS
#define LDAL_EXT
#else
#define LDAL_EXT                 extern
#endif

#define LDAL_DEBUG               0  /* 0: disable, 1: enable */
#define debug_print(fmt, args...) \
    do { if(LDAL_DEBUG) \
    printf(fmt, ## args); \
    } while(0)

#define LOG_D(...)               printf(__VA_ARGS__);
#define LOG_E(...)               printf(__VA_ARGS__);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)          (sizeof(arr) / sizeof((arr)[0]))
#endif


#ifdef __cplusplus
}
#endif

#endif /* __LDAL_PRIVATE_H__ */