/*************************************************************************
    > File Name: stream_reader.h
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 13时40分26秒
 ************************************************************************/


#ifndef INSDCXMLPARSER_STREAM_READER_H
#define INSDCXMLPARSER_STREAM_READER_H

#include <stdint.h>
#include <string.h>
#include "utils.h"

/* the buffer_size of the cache (128MB) */
#define BUFFER_SIZE 134217728


/*! @typedef buffer_t
  @abstract the buffer for xml stream
  @field  size           the size of the current available data
  @field  capacity       the size of the buffer
  @field  start_tag      the start tag of the data body (e.g. <BioSample)
  @field  end_tag        the end tag of the data body (e.g. </BioSample)
  @field  front          the pointer to the next round searching in the buffer
  @field  data           the pointer to the data from file
 */
typedef struct {
    uint32_t size;
    uint32_t capacity;
    kstring_t start_tag;
    kstring_t end_tag;
    char *front;
    char *data;
} buffer_t;


/*! @typedef body_t
  @abstract the data body index in the buffer
  @field  start             the start pointer of the data body in the buffer
  @field  id                the id of the data body
  @field  size              the size of the data body
 */
typedef struct {
    char *start;
    uint32_t id;
    uint32_t size;
} body_t;


/*! @typedef cache_t
  @abstract the cache used to parse xml file and store with their index in the buffer
  @field  size              the number of item in the item_list
  @field  capacity          the max number of items allowed to store (with memory allocated to item_list)
  @field  item_list         the item list with data body
  @field  buffer            the buffer used to cache stream data from file
  @field  file_hd           the file handle by POSIX open function
 */
typedef struct {
    uint32_t size;
    uint32_t capacity;
    body_t *item_list;
    buffer_t buffer;
    int file_hd;
} cache_t;


/*! @function: initiation of stream cache
  @param  filename           the filename of the XML file
  @param  start_tag          the start tag in the XML to catch
  @param  end_tag            the end tag in the XML to catch
  @return                    cache object
 */
cache_t *stream_cache_init(const char *filename, const char *start_tag, const char *end_tag);


/*! @function: destroy the memory allocated to cache
  @param  cache              the cache object from stream_cache_init
  @return
 */
void stream_cache_destroy(cache_t * cache);


/*! @function: caching and parsing XML file
  @param  cache              the cache object from stream_cache_init
  @return                    status of caching (-1: end of the stream)
 */
int stream_cache_data(cache_t *cache);


#endif //INSDCXMLPARSER_STREAM_READER_H