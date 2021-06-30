#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "ldal.h"
#include "at.h"

#define LOG_D(...)         printf(__VA_ARGS__);
#define LOG_E(...)         printf(__VA_ARGS__);

#if 0
static time_t get_current_time_with_ms(void)
{
    time_t          ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return ms;
}
#else
static time_t get_current_time_with_ms(void)
{
    time_t ms;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // convert tv_sec & tv_usec to millisecond
    ms = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    return ms;
}
#endif

/**
 * Create response object.
 *
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *         = 0: the response data will auto return when received 'OK' or 'ERROR'
 *        != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return != RT_NULL: response object
 *          = RT_NULL: no memory
 */
at_response_t at_create_resp(size_t buf_size, size_t line_num, int32_t timeout)
{
    at_response_t resp = NULL;

    resp = (at_response_t) calloc(1, sizeof(struct at_response));
    if (resp == NULL) {
        LOG_E("AT create response object failed! No memory for response object!");
        return NULL;
    }

    resp->buf = (char *) calloc(1, buf_size);
    if (resp->buf == NULL) {
        LOG_E("AT create response object failed! No memory for response buffer!");
        free(resp);
        return NULL;
    }

    resp->buf_size = buf_size;
    resp->line_num = line_num;
    resp->line_counts = 0;
    resp->timeout = timeout;

    return resp;
}

/**
 * Delete and free response object.
 *
 * @param resp response object
 */
void at_delete_resp(at_response_t resp)
{
    if (resp && resp->buf) {
        free(resp->buf);
    }

    if (resp) {
        free(resp);
        resp = NULL;
    }
}

/**
 * Set response object information
 *
 * @param resp response object
 * @param buf_size the maximum response buffer size
 * @param line_num the number of setting response lines
 *         = 0: the response data will auto return when received 'OK' or 'ERROR'
 *        != 0: the response data will return when received setting lines number data
 * @param timeout the maximum response time
 *
 * @return  != RT_NULL: response object
 *           = RT_NULL: no memory
 */
at_response_t at_resp_set_info(at_response_t resp, size_t buf_size, size_t line_num, int32_t timeout)
{
    char *p_temp;
    assert(resp);

    if (resp->buf_size != buf_size)
    {
        resp->buf_size = buf_size;

        p_temp = (char *) realloc(resp->buf, buf_size);
        if (p_temp == NULL) {
            LOG_D("No memory for realloc response buffer size(%lu).", buf_size);
            return NULL;
        } else {
            resp->buf = p_temp;
        }
    }

    resp->line_num = line_num;
    resp->timeout = timeout;

    return resp;
}

/**
 * Get one line AT response buffer by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 *
 * @return != RT_NULL: response line buffer
 *          = RT_NULL: input response line error
 */
const char *at_resp_get_line(at_response_t resp, size_t resp_line)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    size_t line_num = 1;

    assert(resp);

    if (resp_line > resp->line_counts || resp_line <= 0) {
        LOG_E("AT response get line failed! Input response line(%ld) error!", resp_line);
        return NULL;
    }

    for (line_num = 1; line_num <= resp->line_counts; line_num++) {
        if (resp_line == line_num) {
            resp_line_buf = resp_buf;
            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}

/**
 * Get one line AT response buffer by keyword
 *
 * @param resp response object
 * @param keyword query keyword
 *
 * @return != RT_NULL: response line buffer
 *          = RT_NULL: no matching data
 */
const char *at_resp_get_line_by_kw(at_response_t resp, const char *keyword)
{
    char *resp_buf = resp->buf;
    char *resp_line_buf = NULL;
    size_t line_num = 1;

    assert(resp);
    assert(keyword);

    for (line_num = 1; line_num <= resp->line_counts; line_num++) {
        if (strstr(resp_buf, keyword)) {
            resp_line_buf = resp_buf;
            return resp_line_buf;
        }

        resp_buf += strlen(resp_buf) + 1;
    }

    return NULL;
}

/**
 * Get and parse AT response buffer arguments by line number.
 *
 * @param resp response object
 * @param resp_line line number, start from '1'
 * @param resp_expr response buffer expression
 *
 * @return -1 : input response line number error or get line buffer error
 *          0 : parsed without match
 *         >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args(at_response_t resp, size_t resp_line, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    assert(resp);
    assert(resp_expr);

    if ((resp_line_buf = at_resp_get_line(resp, resp_line)) == NULL) {
        return -1;
    }

    va_start(args, resp_expr);
    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);
    va_end(args);

    return resp_args_num;
}

/**
 * Get and parse AT response buffer arguments by keyword.
 *
 * @param resp response object
 * @param keyword query keyword
 * @param resp_expr response buffer expression
 *
 * @return -1 : input keyword error or get line buffer error
 *          0 : parsed without match
 *         >0 : the number of arguments successfully parsed
 */
int at_resp_parse_line_args_by_kw(at_response_t resp, const char *keyword, const char *resp_expr, ...)
{
    va_list args;
    int resp_args_num = 0;
    const char *resp_line_buf = NULL;

    assert(resp);
    assert(resp_expr);

    if ((resp_line_buf = at_resp_get_line_by_kw(resp, keyword)) == NULL) {
        return -1;
    }

    va_start(args, resp_expr);
    resp_args_num = vsscanf(resp_line_buf, resp_expr, args);
    va_end(args);

    return resp_args_num;
}

/**
 * Send commands to AT server and wait response.
 *
 * @param client current AT client object
 * @param resp AT response object, using RT_NULL when you don't care response
 * @param cmd_expr AT commands expression
 *
 * @return 0 : success
 *        -1 : response status error
 *        -2 : wait timeout
 *        -7 : enter AT CLI mode
 */
int at_obj_exec_cmd(struct ldal_me_device *client, at_response_t resp, const char *cmd_expr, ...)
{
    va_list args;
    size_t cmd_size = 0;
    int result = LDAL_EOK;
    const char *cmd = NULL;

    RT_ASSERT(cmd_expr);

    if (client == NULL)
    {
        LOG_E("input AT Client object is NULL, please create or get AT Client object!");
        return -LDAL_ERROR;
    }

    /* check AT CLI mode */
    if (client->status == AT_STATUS_CLI && resp)
    {
        return -LDAL_EBUSY;
    }

    pthread_mutex_lock(&client->lock);

    client->resp_status = AT_RESP_OK;
    client->resp = resp;

    if (resp != NULL) {
        resp->buf_len = 0;
        resp->line_counts = 0;
    }

    va_start(args, cmd_expr);
    at_vprintfln(client->device, cmd_expr, args);
    va_end(args);

    if (resp != NULL)
    {
        /* int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
           struct timespec {
               time_t tv_sec;      // Seconds
               long   tv_nsec;     // Nanoseconds [0 .. 999999999]
           };
        */
        if (sem_timedwait(&client->resp_notice, resp->timeout) != LDAL_EOK)
        {
            cmd = at_get_last_cmd(&cmd_size);
            LOG_D("execute command (%.*s) timeout (%d ticks)!", cmd_size, cmd, resp->timeout);
            client->resp_status = AT_RESP_TIMEOUT;
            result = -LDAL_ETIMEOUT;
            goto __exit;
        }
        if (client->resp_status != AT_RESP_OK)
        {
            cmd = at_get_last_cmd(&cmd_size);
            LOG_E("execute command (%.*s) failed!", cmd_size, cmd);
            result = -LDAL_ERROR;
            goto __exit;
        }
    }

__exit:
    client->resp = NULL;

    pthread_mutex_unlock(&client->lock);

    return result;
}
