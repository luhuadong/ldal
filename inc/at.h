#ifndef __LDAL_AT_H__
#define __LDAL_AT_H__

#include <stdint.h>
#include <stddef.h>  /* size_t */


#ifdef __cplusplus
extern "C" {
#endif

#define AT_RECV_BUFF_SIZE              2048

#define AT_CMD_NAME_LEN                16
#define AT_END_MARK_LEN                4

#ifndef AT_CMD_MAX_LEN
#define AT_CMD_MAX_LEN                 128
#endif

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_FAIL               "FAIL"
#define AT_END_CR_LF                   "\r\n"
#define AT_CMD_END_MARK                "\r\n"

enum at_status
{
    AT_STATUS_UNINITIALIZED = 0,
    AT_STATUS_INITIALIZED,
    AT_STATUS_CLI,
};
typedef enum at_status at_status_t;

enum at_resp_status
{
    AT_RESP_OK = 0,                   /* AT response end is OK */
    AT_RESP_ERROR = -1,               /* AT response end is ERROR */
    AT_RESP_TIMEOUT = -2,             /* AT response is timeout */
    AT_RESP_BUFF_FULL= -3,            /* AT response buffer is full */
};
typedef enum at_resp_status at_resp_status_t;

struct at_response
{
    /* response buffer */
    char *buf;
    /* the maximum response buffer size, it set by `at_create_resp()` function */
    size_t buf_size;
    /* the length of current response buffer */
    size_t buf_len;
    /* the number of setting response lines, it set by `at_create_resp()` function
     * == 0: the response data will auto return when received 'OK' or 'ERROR'
     * != 0: the response data will return when received setting lines number data */
    size_t line_num;
    /* the count of received response lines */
    size_t line_counts;
    /* the maximum response time */
    int32_t timeout;
};

typedef struct at_response *at_response_t;


/* AT response object create and delete */
at_response_t at_create_resp(size_t buf_size, size_t line_num, int32_t timeout);
void at_delete_resp(at_response_t resp);
at_response_t at_resp_set_info(at_response_t resp, size_t buf_size, size_t line_num, int32_t timeout);

/* AT response line buffer get and parse response buffer arguments */
const char *at_resp_get_line(at_response_t resp, size_t resp_line);
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword);
int at_resp_parse_line_args(at_response_t resp, size_t resp_line, const char *resp_expr, ...);
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...);

#include "ldal.h"

struct at_urc
{
    const char *cmd_prefix;
    const char *cmd_suffix;
    void (*func)(struct ldal_device *client, const char *data, size_t size);
};
typedef struct at_urc *at_urc_t;

struct at_urc_table
{
    size_t urc_size;
    const struct at_urc *urc;
};
typedef struct at_urc *at_urc_table_t;

void *at_client_parser(void *client);
int at_obj_exec_cmd(void *client, at_response_t resp, const char *cmd_expr, ...);

#ifdef __cplusplus
}
#endif

#endif /* __LDAL_AT_H__ */
