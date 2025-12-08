/*************************************************************************
    > File Name: xml_compare.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年12月08 11时43分26秒
 ************************************************************************/


#include <omp.h>

#include "md5.h"
#include "database.h"
#include "stream_reader.h"


void xml_compare_core(database_t *database, char *xml_name, char *diff_name, char *start_tag, char *end_tag)
{
    FILE *file_hd = fopen(diff_name, "wb");
    cache_t *cache = stream_cache_init(xml_name, start_tag, end_tag);
    database_t *cache_db = database_init(16);

    char time_buf[32];
    uint32_t n_total_item = 0;
    fprintf(stderr, "[%s] start to compare the difference ...\n", get_current_time(time_buf));

    while (stream_cache_data(cache) >= 0) {
        database_resize(cache_db, cache->size);

        /* calculate the md5 value in parallel with openmp */
        #pragma omp parallel for shared(cache, cache_db, database)
        for (int i=0; i < cache->size; i++) {
            uint8_t md5_str[16];
            body_t *body = &cache->item_list[i];

            md5_calculate_block((uint8_t *)body->start, body->size, md5_str);
            database_add(cache_db, i, md5_str);  /* Note: the flag value is ignored */
        }

        /* get the different data body by comparing sample database */
        for (int i=0; i < cache->size; i++) {
            body_t *body = &cache->item_list[i];
            uint8_t *raw_md5 = database_query(database, body->id);
            uint8_t *cur_md5 = database_query(cache_db, i);

            if (database->flags[body->id] == 0) {  /* the item is new added */
                fwrite(body->start, sizeof(char), body->size, file_hd);
                fwrite("\n", sizeof(char), 1, file_hd);
                database->flags[body->id] = 3;
                memcpy(raw_md5, cur_md5, 16 * sizeof(uint8_t));
                continue;
            }

            if (strncmp((char *)raw_md5, (char *)cur_md5, 16) != 0) {  /* the item is changed */
                fwrite(body->start, sizeof(char), body->size, file_hd);
                fwrite("\n", sizeof(char), 1, file_hd);
                database->flags[body->id] = 4;
                memcpy(raw_md5, cur_md5, 16 * sizeof(uint8_t));
                continue;
            }

            database->flags[body->id] = 2;  /* the item is unchanged */
        }

        n_total_item += cache->size;
        fprintf(stderr, "\r[*] compare number of items: %d", n_total_item);
    }
    fclose(file_hd);
    fprintf(stderr, "\n[%s] done!\n", get_current_time(time_buf));
}


void diff_list_write(const database_t *database, const char *diff_list)
{
    FILE *file_hd = fopen(diff_list, "wb");

    /* output the status (change, add, delete) to stander output */
    uint8_t *flags = database->flags;
    static const char *table[] = {NULL, "DELETE", NULL, "ADD", "CHANGE"};

    for (uint32_t id=0; id < database->capacity; id++) {
        if (table[flags[id]] == NULL)  /* 0:unused and 2:unchanged */
            continue;

        fprintf(file_hd, "%s\t%d\n", table[flags[id]], id);
    }
    fclose(file_hd);
}


void sample_xml_compare(const args_t *args)
{
    database_t *database = database_load(args->database);

    if (strcmp(database->db_type, "SAMPLE") != 0) {
        fprintf(stderr, "[Error:%s] conflict database type: %s!\n", __func__, database->db_type);
        exit(-1);
    }

    if (args->xml_date <= database->db_date) {
        fprintf(stderr, "[Error:%s] conflict date detected between the database and given xml file!\n", __func__);
        fprintf(stderr, "  (-) database date: %d\n", database->db_date);
        fprintf(stderr, "  (-) the xml date: %d\n", args->xml_date);
        exit(-1);
    }

    /* parse the difference of the xml file */
    char path_buf[512];

    snprintf(path_buf, sizeof(path_buf), "%s/sample_diff.xml", args->output_dir);
    xml_compare_core(database, args->xml_file, path_buf, SAMPLE_START_TAG, SAMPLE_END_TAG);

    snprintf(path_buf, sizeof(path_buf), "%s/sample_diff.list", args->output_dir);
    diff_list_write(database, path_buf);

    /* update the database to current date */
    database->db_date = args->xml_date;
    database_update(database, args->database);
}


void project_xml_compare(const args_t *args)
{
    database_t *database = database_load(args->database);

    if (strcmp(database->db_type, "PROJECT") != 0) {
        fprintf(stderr, "[Error:%s] conflict database type: %s!\n", __func__, database->db_type);
        exit(-1);
    }

    if (args->xml_date <= database->db_date) {
        fprintf(stderr, "[Error:%s] conflict date detected between the database and given xml file!\n", __func__);
        fprintf(stderr, "  (-) database date: %d\n", database->db_date);
        fprintf(stderr, "  (-) the xml date: %d\n", args->xml_date);
        exit(-1);
    }

    /* parse the difference of the xml file */
    char path_buf[512];

    snprintf(path_buf, sizeof(path_buf), "%s/project_diff.xml", args->output_dir);
    xml_compare_core(database, args->xml_file, path_buf, PROJECT_START_TAG, PROJECT_END_TAG);

    snprintf(path_buf, sizeof(path_buf), "%s/project_diff.list", args->output_dir);
    diff_list_write(database, path_buf);

    /* update the database to current date */
    database->db_date = args->xml_date;
    database_update(database, args->database);
}
