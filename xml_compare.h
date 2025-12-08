/*************************************************************************
    > File Name: xml_compare.h
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年12月08 11时43分26秒
 ************************************************************************/

#ifndef INSDCXMLPARSER_XML_COMPARE_H
#define INSDCXMLPARSER_XML_COMPARE_H

#include "params.h"


/*! @function: compare the difference between database and the current biosample
  @param  args               the command line parameters
  @return
 */
void sample_xml_compare(args_t *args);


/*! @function: compare the difference between database and the current bioproject
  @param  args               the command line parameters
  @return
 */
void project_xml_compare(args_t *args);


#endif //INSDCXMLPARSER_XML_COMPARE_H