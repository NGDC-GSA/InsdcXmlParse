/*************************************************************************
    > File Name: database.h
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年12月08 10时18分26秒
 ************************************************************************/

#ifndef INSDCXMLPARSER_DATABASE_H
#define INSDCXMLPARSER_DATABASE_H

#include <stdint.h>
#include "params.h"

/* the maximum ID (usually more bigger) for the sample table */
#define SAMPLE_TABLE_SIZE 60000000

/* the start and end tag for biosample */
#define SAMPLE_START_TAG "<BioSample "
#define SAMPLE_END_TAG "</BioSample>"

/* the maximum ID (usually more bigger) for the project table */
#define PROJECT_TABLE_SIZE 2000000

/* the start and end tag for bioproject */
#define PROJECT_START_TAG "<Package>"
#define PROJECT_END_TAG "</Package>"


/*! @typedef database_t
  @abstract the database used to store the MD5 value for given ID
  @field  db_type           the database type, could be SAMPLE or PROJECT
  @field  db_date           the date of the current database
  @field  capacity          the maximum number of items to store
  @field  flags             the status after compare (0:empty, 1:delete, 2:constant, 3:add, 4:modify)
  @field  values            the value list used to store the MD5 (16 uint8_t for one MD5)
 */
typedef struct {
    char db_type[8];
    uint32_t db_date;
    uint32_t capacity;
    uint8_t *flags;
    uint8_t *values;
} database_t;


/*! @function: initiation of database
  @param  max_size           the maximum number of items for the table to store
  @return                    database object
 */
database_t *database_init(uint32_t max_size);


/*! @function: resize the database memory
  @param  database           the pointer to the database object
  @param  new_size           the new size needed to store all data
  @return                    database object
 */
database_t *database_resize(database_t *database, uint32_t new_size);


/*! @function: database build
  @param   args              the args necessary for build the database
  @return                    the database object
 */
int database_build(const args_t *args);


/*! @function: database update and save
  @param   database          the pointer to the database object
  @param   file_name         the database file name
  @return
 */
void database_update(const database_t *database, const char *file_name);


/*! @function: database load
  @param   file_name         the database file name
  @return  database          the pointer to the database object
 */
database_t *database_load(char *file_name);


/*! @function: get the address of the MD5 value for given index
  @param  _database          the pointer to the database object
  @param  _index             the index to store the MD5 value (only 16 bytes could be use)
  @param  _value             the MD5 value to store
  @return
 */
#define database_add(_database, _index, _value) do {     \
    uint8_t *dest = (_database)->values + (_index<<4);   \
    memcpy(dest, _value, 16 * sizeof(unsigned char));    \
    (_database)->flags[_index] = 1;                      \
} while(0)


/*! @function: get the address of the MD5 value for given index
  @param  _database          the pointer to the database object
  @param  _index             the index to store the MD5 value (only 16 bytes could be use)
  @param  _value             the MD5 value to store
  @return
 */
#define database_query(_database, _index) ((_database)->values + (_index<<4))


#endif //INSDCXMLPARSER_DATABASE_H