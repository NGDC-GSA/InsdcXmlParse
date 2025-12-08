/*************************************************************************
    > File Name: xml_parser.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年11月27 13时44分26秒
 ************************************************************************/

#include <stdio.h>

#include "params.h"
#include "database.h"
#include "xml_compare.h"


int main(int argc, char **argv)
{
    args_t *args;
    args = params_parse(argc, argv);

    switch (args->params_mode) {
        case PARAMS_BUILD:
            database_build(args);
            break;

        case PARAMS_SAMPLE:
            sample_xml_compare(args);
            break;

        case PARAMS_PROJECT:
            project_xml_compare(args);
            break;

        default:
            fprintf(stderr, "[Error:%s] Trust me, you will never be here!\n\n", __func__);
    }

    return 0;
}
