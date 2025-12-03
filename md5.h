/* Calculate the md5 value of the given file
 * Source code: https://github.com/chinaran/Compute-file-or-string-md5
 * Modified: xiaolong zhang (xiaolongzhang2015@163.com)
 * Date: 2023-03-29
 * */

#ifndef MD5_H
#define MD5_H

#define MD5_SIZE  16
#define MD5_STR_LEN 32
#define MD5_BUF_SIZE  65536


typedef struct {
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];   
} MD5_CTX;


/* the original basic calculation function */
void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen);
void MD5Final(MD5_CTX *context, unsigned char digest[16]);
void MD5Transform(unsigned int state[4], unsigned char block[64]);
void MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
void MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);


/*! @function: calculate the md5 value of the given data block
  @param    block_data    a data block that needs to be calculated for md5 value
  @param    data_size     number of bytes of the given block_data
  @param    md5_value     the original md5 value (32 4-bit integer)
  @return                 status of the function
 */
int md5_calculate_block(unsigned char *block_data, unsigned int data_size, unsigned char *md5_value);


/*! @function: calculate the md5 value of the given file
  @param    file_name     the input filename
  @param    md5_value     the original md5 value (32 4-bit integer)
  @return                 status of the function
 */
int md5_calculate_file(const char *file_name, unsigned char *md5_value);


/*! @function: convert the md5 value to string with 32-bytes
  @param    md5_value     the original md5 value (32 4-bit integer)
  @param    md5_str       the converted md5 string (32 bytes + '\0')
  @return                 the pointer to the md5_str
 */
char *md5_to_string(const unsigned char *md5_value, char *md5_str);


#endif  //MD5_H

