/*************************************************************************
    > File Name: utils.h
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 18时03分26秒
 ************************************************************************/


#ifndef INSDCXMLPARSER_UTILS_H
#define INSDCXMLPARSER_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#ifndef kroundup32
#define kroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef KSTRING_T
#define KSTRING_T kstring_t
typedef struct __kstring_t {
    size_t l, m;
    char *s;
} kstring_t;
#endif


/* safely open a text file */
#ifndef err_open
#define err_open(_fp, _fn, _mode) do {                                  \
    _fp = fopen(_fn, _mode);                                            \
    if (!_fp) {                                                         \
        fprintf(stderr, "\n[SysError:err_open]: failed to open file of %s!\n", _fn); exit(-1); \
    }                                                                   \
} while(0)
#endif


/* safely malloc memory for type */
#ifndef err_malloc
#define err_malloc(_p, _n, _type) do {                                  \
    _type *tem = (_type *)malloc((_n) * sizeof(_type));                 \
    if (!tem) {                                                         \
        fprintf(stderr, "\n[SysError:err_malloc]: failed to malloc memory!\n"); exit(-1); \
    } (_p) = tem;                                                       \
} while(0)
#endif


/* safely calloc memory for type */
#ifndef err_calloc
#define err_calloc(_p, _n, _type) do {                                  \
    _type *tem = (_type *)calloc((_n), sizeof(_type));                  \
    if (!tem) {                                                         \
        fprintf(stderr, "\n[SysError:err_calloc]: failed to calloc memory!\n"); exit(-1); \
    } (_p) = tem;                                                       \
} while(0)
#endif

/* safely realloc memory for type */
#ifndef err_realloc
#define err_realloc(_p, _n, _type) do {                                 \
    _type *tem = (_type *)realloc((_p), (_n) * sizeof(_type));          \
    if (!tem) {                                                         \
        fprintf(stderr, "\n[SysError:err_realloc]: failed to realloced memory!\n"); exit(-1); \
    } (_p) = tem;                                                       \
} while(0)
#endif


/* copy the content of the src to kdest */
kstring_t *k_strcpy(kstring_t *kdest, const char *src);


/* copy n bytes from src to kdest */
kstring_t *k_strncpy(kstring_t *kdest, const char *src, size_t n);


/* destroy the k_string and free the allocated memory */
void k_strfree(kstring_t *kstr);

/* get the current time */
char *get_current_time(char *time_buf);


/* check whether the file is existed */
int32_t is_file_exists(char *file_fn);


#endif //INSDCXMLPARSER_UTILS_H