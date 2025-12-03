/*************************************************************************
    > File Name: utils.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 18时03分26秒
 ************************************************************************/


#include <time.h>
#include <string.h>
#include "utils.h"


kstring_t *k_strcpy(kstring_t *kdest, const char *src)
{
    size_t src_len;
    src_len = strlen(src) + 1;

    if (kdest->m < src_len) {
        kdest->m = src_len; kroundup32(kdest->m);
        err_realloc(kdest->s, kdest->m, char);
    }
    kdest->l = src_len-1;
    strcpy(kdest->s, src);
    return kdest;
}


kstring_t *k_strncpy(kstring_t *kdest, const char *src, size_t n)
{
    size_t src_len;
    src_len = n + 1;

    if (kdest->m < src_len) {
        kdest->m = src_len; kroundup32(kdest->m);
        err_realloc(kdest->s, kdest->m, char);
    }
    kdest->l = n;
    memcpy(kdest->s, src, kdest->l*sizeof(char));
    kdest->s[kdest->l] = '\0';
    return kdest;
}


void k_strfree(kstring_t *kstr)
{
    if (kstr == NULL)  /* empty kstring object */
        return ;

    if (kstr->s != NULL) {
        free(kstr->s); kstr->m = kstr->l = 0;
        kstr->s = NULL;
    }

    kstr = NULL;
    free(kstr);
}


char *get_current_time(char *time_buf)
{
    time_t c_time;

    time(&c_time);
    const struct tm *tm_obj = gmtime(&c_time);

    const int year = tm_obj->tm_year + 1900;
    const int month = tm_obj->tm_mon + 1;
    const int day = tm_obj->tm_mday;
    const int hour = tm_obj->tm_hour + 8;
    const int minute = tm_obj->tm_min;
    const int second = tm_obj->tm_sec;

    sprintf(time_buf, "%d-%d-%d %d:%d:%d", year, month, day, hour, minute, second);
    return time_buf;
}


int32_t is_file_exists(char *file_fn)
{
    FILE *file_fp = fopen(file_fn, "r");

    if (file_fp != NULL) {
        fclose(file_fp); return 1;
    }

    return 0;
}
