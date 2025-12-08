/*************************************************************************
    > File Name: database.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年12月08 10时18分26秒
 ************************************************************************/

#include "md5.h"
#include "stream_reader.h"
#include "database.h"


database_t *database_init(uint32_t max_size)
{
    database_t *database;

    err_calloc(database, 1, database_t);
    database->capacity = max_size;

    /* allocate memory for md5 values and flags */
    err_calloc(database->flags, database->capacity, uint8_t);
    err_calloc(database->values, database->capacity<<4, uint8_t);  /* 16 uint8_t for one MD5 */

    return database;
}


database_t *database_resize(database_t *database, uint32_t new_size)
{
    if (database->capacity >= new_size)
        return database;

    /* expand the database memory */
    uint32_t old_capacity = database->capacity;
    database->capacity = new_size; kroundup32(database->capacity);

    /* expand the flags space and memset the new allocated space to 0 */
    err_realloc(database->flags, database->capacity, uint8_t);
    memset(database->flags+old_capacity, 0, database->capacity-old_capacity);

    /* expand the md5 values and memset the new allocated space to 0 */
    err_realloc(database->values, database->capacity<<4, uint8_t);
    memset(database->values+(old_capacity<<4), 0, (database->capacity-old_capacity)<<4);

    return database;
}


static void database_build_core(database_t *database, const char *xml_file, const char *start_tag, const char *end_tag)
{
    /* cache and table object initiation */
    uint32_t n_total_item = 0;
    char time_buf[32];
    cache_t *cache = stream_cache_init(xml_file, start_tag, end_tag);

    fprintf(stderr, "[%s] start to build the database ...\n", get_current_time(time_buf));
    while (stream_cache_data(cache) >= 0) {

        #pragma omp parallel for shared(cache, database)
        for (int i=0; i < cache->size; i++) {
            uint8_t md5_str[16];
            body_t *body = &cache->item_list[i];

            md5_calculate_block((uint8_t *)body->start, body->size, md5_str);
            database_add(database, body->id, md5_str);
        }
        n_total_item += cache->size;
        fprintf(stderr, "\r[*] parse number of items: %d", n_total_item);
    }

    stream_cache_destroy(cache);  /* destroy the memory of cache */
    fprintf(stderr, "\n[*] database version: %s (%d)\n", database->db_type, database->db_date);
    fprintf(stderr, "[%s] done!\n", get_current_time(time_buf));
}


static void database_save(const database_t *database, const char *file_name)
{
    FILE *file_hd = fopen(file_name, "wb");

    if (file_hd == NULL) {
        fprintf(stderr, "[Error:%s]: failed to open (%s)!\n", __func__, file_name);
        exit(-1);
    }

    /* save the database type and date */
    fwrite(database->db_type, sizeof(char), 8, file_hd);
    fwrite(&database->db_date, sizeof(uint32_t), 1, file_hd);
    fwrite(&database->capacity, sizeof(uint32_t), 1, file_hd);

    /* save the flags of the database */
    fwrite(database->flags, sizeof(uint8_t), database->capacity, file_hd);
    fwrite(database->values, sizeof(uint8_t), database->capacity<<4, file_hd);

    /* close the file handle */
    fclose(file_hd);
}


int database_build(const args_t *args)
{
    uint32_t table_size = strcmp(args->xml_type, "SAMPLE") ? PROJECT_TABLE_SIZE : SAMPLE_TABLE_SIZE;
    database_t *database = database_init(table_size);

    /* set the database type and database date */
    strcpy(database->db_type, args->xml_type);
    database->db_date = args->xml_date;

    if (strcmp(args->xml_type, "SAMPLE") == 0)
        database_build_core(database, args->xml_file, SAMPLE_START_TAG, SAMPLE_END_TAG);

    else  // PROJECT
        database_build_core(database, args->xml_file, PROJECT_START_TAG, PROJECT_END_TAG);

    /* save the database file */
    database_save(database, args->database);
    return 0;
}


/* func: reset the flags after comparing with new xml file
 *
 *               unchange  add  modify  delete  unused
 *    raw_flag      1       0     1       1       0       // previous database flag
 *    cur_flag      2       3     4       1       0       // after compare with new xml file
 * update_flag      1       1     1       0       0       // after update the database
 */
void database_update(const database_t *database, const char *file_name)
{
    uint8_t *flags = database->flags;
    static const uint8_t table[8] = {0, 0, 1, 1, 1, 0, 0, 0};

    /* update the flag before save the database */
    for (uint32_t id=0; id < database->capacity; id++) {
        if (flags[id] == 1)   /* the item is deleted from database */
            memset(&database->values[id<<4], 0, 16);

        flags[id] = table[flags[id]];
    }

    /* save the database */
    database_save(database, file_name);
}


database_t *database_load(char *file_name)
{
    FILE *file_hd = fopen(file_name, "rb");

    if (file_hd == NULL) {
        fprintf(stderr, "[Error:%s]: failed to open (%s)!\n", __func__, file_name);
        exit(-1);
    }

    char time_buf[32];
    fprintf(stderr, "[%s] start to load the database ...\n", get_current_time(time_buf));

    /* read the database data from file */
    char db_type[8];
    uint32_t data[2];  // [db_date, capacity]
    size_t n_item;

    n_item = fread(db_type, sizeof(char), 8, file_hd);
    if (n_item != 8) goto _truncated_error;

    n_item = fread(data, sizeof(uint32_t), 2, file_hd);
    if (n_item != 2) goto _truncated_error;

    /* initiate the database */
    database_t *database = database_init(data[1]);
    database->db_date = data[0];
    strcpy(database->db_type, db_type);

    /* read the flags and md5 value list */
    n_item = fread(database->flags, sizeof(uint8_t), database->capacity, file_hd);
    if (n_item != database->capacity) goto _truncated_error;

    n_item = fread(database->values, sizeof(uint8_t), database->capacity<<4, file_hd);
    if (n_item != database->capacity<<4) goto _truncated_error;

    fprintf(stderr, "[*] database version: %s (%d)\n", db_type, data[0]);
    fprintf(stderr, "[%s] done!\n", get_current_time(time_buf));
    fclose(file_hd);
    return database;

    _truncated_error:
    fprintf(stderr, "[Error:%s] truncated database file (%s) detected!\n\n", __func__, file_name);
    exit(-1);
}
