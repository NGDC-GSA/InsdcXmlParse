/*************************************************************************
    > File Name: xml_parser.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 13时44分26秒
 ************************************************************************/

#include <omp.h>
#include "md5.h"
#include "stream_reader.h"


#define table_memory_resize(_table, _new_size) do {                              \
    if ((_table)->capacity < (_new_size<<4)) {                                   \
        (_table)->capacity = (_new_size<<4); kroundup32((_table)->capacity);     \
        err_realloc((_table)->flags, (_table)->capacity>>4, uint8_t);            \
        err_realloc((_table)->values, (_table)->capacity, uint8_t);              \
    }                                                                            \
} while(0)



table_t *sample_database_build(const char *file_name)
{
    const char *start_tag = "<BioSample ";
    const char *end_tag = "</BioSample>";

    /* cache and table object initiation */
    cache_t *cache = stream_cache_init(file_name, start_tag, end_tag);
    table_t *table = lookup_table_init(TABLE_SIZE);

    char time_buf[32];
    fprintf(stderr, "[%s] start to build the database ...\n", get_current_time(time_buf));

    while (stream_cache_data(cache) >= 0) {
        #pragma omp parallel for shared(cache, table)
        for (int i=0; i < cache->size; i++) {
            uint8_t md5_str[16];
            body_t *body = &cache->item_list[i];

            md5_calculate_block((uint8_t *)body->start, body->size, md5_str);
            lookup_table_add(table, body->id, md5_str);
        }
        table->size += cache->size;
        fprintf(stderr, "\r[*] parse number of items: %d", table->size);
    }
    stream_cache_destroy(cache);  /* destroy the memory of cache */
    fprintf(stderr, "\n[%s] done!\n", get_current_time(time_buf));

    return table;
}


void sample_diff_compare(const table_t *table, const char *file_name, const char *diff_name)
{
    const char *start_tag = "<BioSample ";
    const char *end_tag = "</BioSample>";

    FILE *file_hd = fopen(diff_name, "wb");
    cache_t *cache = stream_cache_init(file_name, start_tag, end_tag);
    table_t *md5_table = lookup_table_init(8);

    char time_buf[32];
    uint32_t n_total_item = 0;
    fprintf(stderr, "[%s] start to compare the difference ...\n", get_current_time(time_buf));

    while (stream_cache_data(cache) >= 0) {
        table_memory_resize(md5_table, cache->size);

        /* calculate the md5 value in parallel with openmp */
        #pragma omp parallel for shared(cache, table)
        for (int i=0; i < cache->size; i++) {
            uint8_t md5_str[16];
            body_t *body = &cache->item_list[i];

            md5_calculate_block((uint8_t *)body->start, body->size, md5_str);
            lookup_table_add(md5_table, i, md5_str);  /* Note: the flag value is ignored */
        }

        /* get the different data body by comparing sample database */
        for (int i=0; i < cache->size; i++) {
            body_t *body = &cache->item_list[i];
            uint8_t *raw_md5 = lookup_table_query(table, body->id);
            uint8_t *cur_md5 = lookup_table_query(md5_table, i);

            if (table->flags[body->id] == 0) {  /* the item is new added */
                table->flags[body->id] = 3;
                fwrite(body->start, sizeof(char), body->size, file_hd);
                fwrite("\n", sizeof(char), 1, file_hd);
                continue;
            }

            if (strncmp((char *)raw_md5, (char *)cur_md5, 16) != 0) {  /* the item is changed */
                table->flags[body->id] = 4;
                fwrite(body->start, sizeof(char), body->size, file_hd);
                fwrite("\n", sizeof(char), 1, file_hd);
                continue;
            }

            table->flags[body->id] = 2;  /* the item keep constant */
        }

        n_total_item += cache->size;
        fprintf(stderr, "\r[*] compare number of items: %d", n_total_item);
    }
    fclose(file_hd);
    fprintf(stderr, "\n[%s] done!\n", get_current_time(time_buf));

    /* output the status (change, add, delete) to stander output */
    fprintf(stderr, "[%s] start to output item status ...\n", get_current_time(time_buf));
    uint8_t *flags = table->flags;
    static const char *flag_table[] = {NULL, "DELETE", NULL, "ADD", "CHANGE"};

    for (uint32_t id=0, total=table->capacity>>4; id < total; id++) {
        if (flag_table[flags[id]] == NULL)  /* 0:empty and 2:constant */
            continue;

        fprintf(stdout, "%s\t%d\n", flag_table[flags[id]], id);
    }
    fflush(stdout);
    fprintf(stderr, "[%s] done!\n", get_current_time(time_buf));
}


int main()
{
    const char *old_file = "/data/insdc/ncbi_update_day/sample/20251128/biosample_set.xml";
    const char *new_file = "/data/insdc/ncbi_update_day/sample/20251129/biosample_set.xml";
    const char *diff_file = "/data/insdc/ncbi_update_day/sample/20251129/biosample_diff.xml";

    // const char *filename = "/data/insdc/ncbi_update_day/project/20251129/bioproject.xml";
    // const char *start_tag = "<Package>";
    // const char *end_tag = "</Package>";

    table_t *sample_table = sample_database_build(old_file);
    sample_diff_compare(sample_table, new_file, diff_file);
}