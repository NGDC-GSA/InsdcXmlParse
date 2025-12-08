/*************************************************************************
    > File Name: stream_reader.cpp
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 13时44分26秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
#include "stream_reader.h"


#define cache_memory_resize(_cache) do {                                           \
    if ((_cache)->size == (_cache)->capacity) {                                    \
        (_cache)->capacity = (_cache)->capacity ? (_cache)->capacity << 1 : 1024;  \
        err_realloc((_cache)->item_list, (_cache)->capacity, body_t);              \
    }                                                                              \
} while(0)


#define cache_buffer_reset(_buffer) do {                                           \
    memmove((_buffer)->data, (_buffer)->front, (_buffer)->size * sizeof(char));    \
    (_buffer)->data[(_buffer)->size] = '\0';                                       \
    (_buffer)->front = (_buffer)->data;                                            \
} while(0)


cache_t *stream_cache_init(const char *filename, const char *start_tag, const char *end_tag)
{
    cache_t *cache;
    err_calloc(cache, 1, cache_t);

    /* open the input file */
    cache->file_hd = open(filename, O_RDONLY);
    if (cache->file_hd < 0) {
        fprintf(stderr, "[Error:stream_cache_init] failed to open the file (%s)!\n", filename);
        exit(-1);
    }

    buffer_t *buffer = &cache->buffer;
    k_strcpy(&buffer->start_tag, start_tag);
    k_strcpy(&buffer->end_tag, end_tag);

    if (buffer->start_tag.l >= 64 || buffer->end_tag.l >= 64) {
        fprintf(stderr, "[Error:stream_cache_init] the length of start tag or end tag exceed 64!\n");
        exit(-1);
    }

    /* prepare the data filed of the buffer */
    err_malloc(buffer->data, BUFFER_SIZE + 8, char);
    buffer->capacity = BUFFER_SIZE;
    buffer->front = buffer->data;

    return cache;
}


void stream_cache_destroy(cache_t *cache)
{
    if (cache == NULL) return;

    /* destroy the memory of item_list */
    if (cache->item_list != NULL) free(cache->item_list);

    /* destroy the buffer */
    buffer_t *buffer = &cache->buffer;
    if (buffer == NULL) return;

    k_strfree(&buffer->start_tag);
    k_strfree(&buffer->end_tag);
    if (buffer->data != NULL) free(buffer->data);

    free(cache);
}


static uint32_t stream_id_parse(const char *data, const uint32_t data_size)
{
    const char *id_start = strstr(data, "id=\"");
    uint32_t n_shift = id_start - data;

    if (id_start == NULL || n_shift > data_size) {
        fprintf(stderr, "[Error:stream_id_parse] the id is not exist in the data body!\n");
        exit(-1);
    }

    /* skip the id tag before parsing*/
    id_start += 4;
    uint32_t id = 0;

    while (*id_start >= '0' && *id_start <= '9') {
        id = id * 10 + (*id_start - '0');
        id_start++;
    }

    return id;
}


static int stream_cache_parse(cache_t *cache)
{
    buffer_t *buffer = &cache->buffer;
    kstring_t *st = &buffer->start_tag, *et = &buffer->end_tag;

    /* parse the data body with start and end tag */
    while (1) {
        /* find the start tag */
        char *start = strstr(buffer->front, st->s);
        if (start == NULL) break;  /* there is no start tag in the buffer yet */

        buffer->size -= start - buffer->front;
        buffer->front = start;

        /* find the end tag */
        char *end = strstr(buffer->front + st->l, et->s);
        if (end == NULL) break;  /* there is no end tag in the buffer yet */

        /* store the tag-pair when both start_tag and end_tag were found */
        cache_memory_resize(cache);
        body_t *body = &cache->item_list[cache->size++];
        body->start = start;
        body->size = end - start + et->l;
        body->id = stream_id_parse(body->start, body->size);

        /* shift the front to next tag-pair */
        buffer->front += body->size;
        buffer->size -= body->size;
    }

    return 0;
}


int stream_cache_data(cache_t *cache)
{
    buffer_t *buffer = &cache->buffer;

    /* the start_tag is not existed in the total buffer (invalid tag) */
    if (buffer->size == buffer->capacity) {
        fprintf(stderr, "[Error:stream_cache_data] the tag (%s) may NOT EXIST in your file!\n", buffer->start_tag.s);
        exit(-1);
    }

    cache_buffer_reset(buffer);
    const size_t n_bytes = read(cache->file_hd, buffer->data + buffer->size, buffer->capacity - buffer->size);
    if (n_bytes <= 0) return -1;  /* stream end of the input file */

    buffer->size += n_bytes;
    buffer->data[buffer->size] = '\0';

    /* parse the stream cache to find all potential body data */
    cache->size = 0;
    return stream_cache_parse(cache);
}
