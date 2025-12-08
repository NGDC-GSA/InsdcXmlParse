/*************************************************************************
    > File Name: params.c
    > Author: xlzh
    > Mail: xiaolongzhang2015@163.com
    > Created Time: 2025年12月05 18时15分26秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "params.h"
#include "utils.h"
#include "version.h"


/* copy a string and allocate enough memory (additional 8 bytes for suffix) */
static char *params_str_dup(const char *src_str)
{
    char *dest_str;
    int l_str = (int)strlen(src_str) + 1;

    /* allocate more 8 bytes for potential suffix */
    dest_str = (char *)malloc((l_str+8) * sizeof(char));
    if (!dest_str) {
        fprintf(stderr, "[SysError:%s] failed to malloc memory!\n\n", __func__);
        exit(-1);
    }
    strcpy(dest_str, src_str);
    return dest_str;
}


static void params_show_usage(int params_mode)
{
    const char *usage_main =
        "\nUsage: xml_parser <command> [options]\n"
        "\n"
        "Commands:\n"
        "    build          build database index\n"
        "                   input: xml file of given release date\n"
        "                   output: database file (.db)\n"
        "\n"
        "    sample         parse and compare the difference between database and current xml file\n"
        "                   input: biosample xml file (biosample_set.xml) and the database index\n"
        "                   output: the different data body updated by INSDC\n"
        "\n"
        "    project        parse and compare the difference between database and current xml file\n"
        "                   input: bioproject xml file (bioproject.xml) and the database index\n"
        "                   output: the different data body updated by INSDC\n\n";

    const char *usage_build =
        "\nUsage: xml_parser build [options]\n"
        "\n"
        "Options:\n"
        "    -h|--help                    show help information\n"
        "\n"
        "[Required]\n"
        "    -f|--xml_file      FILE      the xml file used to build the database\n"
        "    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)\n"
        "    -t|--xml_type      STRING    the type of xml file [SAMPLE|PROJECT]\n"
        "    -d|--database      FILE      the output xml database file (.db)\n"
        "\n\n";

    const char *usage_sample =
        "\nUsage: xml_parser sample [options]\n"
        "\n"
        "Options:\n"
        "    -h|--help                    show help information\n"
        "\n"
        "[Required]\n"
        "    -f|--xml_file      FILE      the sample xml file used to compare with the database\n"
        "    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)\n"
        "    -d|--database      FILE      the sample xml database file (.db)\n"
        "    -o|--output_dir    STRING    the output directory\n\n";

    const char *usage_project =
        "\nUsage: xml_parser project [options]\n"
        "\n"
        "Options:\n"
        "    -h|--help                    show help information\n"
        "\n"
        "[Required]\n"
        "    -f|--xml_file      FILE      the project xml file used to compare with the database\n"
        "    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)\n"
        "    -d|--database      FILE      the project xml database file (.db)\n"
        "    -o|--output_dir    STRING    the output directory\n\n";

    fprintf(stderr, "Program: xml_parser (v%s)\n", PARSER_VERSION_STRING);
    fprintf(stderr, "CreateDate: %s\n", PARSER_CREATE_DATE);
    fprintf(stderr, "UpdateDate: %s\n", PARSER_UPDATE_DATE);
    fprintf(stderr, "Author: XiaolongZhang (xiaolongzhang2015@163.com)\n");

    switch (params_mode) {
    case PARAMS_BUILD:
        fprintf(stderr, "%s", usage_build);
        break;

    case PARAMS_SAMPLE:
        fprintf(stderr, "%s", usage_sample);
        break;

    case PARAMS_PROJECT:
        fprintf(stderr, "%s", usage_project);
        break;

    default:
        fprintf(stderr, "%s", usage_main);
        break;
    }
    exit(-1);
}


static const struct option build_options[] =
{
    {"help",  no_argument,  NULL, 'h'},
    {"xml_file", required_argument,  NULL, 'f'},
    {"xml_date",  required_argument,  NULL, 'e'},
    {"xml_type",  required_argument,  NULL, 't'},
    {"database",  required_argument,  NULL, 'd'},
    {NULL,  0,  NULL,  0}
};


static args_t *params_build_parse(int argc, char **argv)
{
    int opt;
    args_t *args;

    /* set the default parameters */
    err_calloc(args, 1, args_t);
    args->params_mode = PARAMS_BUILD;

    /* parse the command line parameters */
    while ( (opt = getopt_long(argc, argv, "f:e:t:d:h", build_options, NULL)) != -1 )
    {
        switch (opt) {
        case 'h':
            args->help = 1;
            params_show_usage(PARAMS_BUILD);
            break;

        case 'f':
            args->xml_file = params_str_dup(optarg);
            break;

        case 'e':
            args->xml_date = (int)strtol(optarg, NULL, 10);
            if (args->xml_date < 20250101 || args->xml_date > 20990101) {
                fprintf(stderr, "[Error:%s] the xml date (%s) is INVALID (e.g. 20251205)!\n\n", __func__, optarg);
                exit(-1);
            }
            break;

        case 't':
            args->xml_type = params_str_dup(optarg);
            if (strcmp(args->xml_type, "SAMPLE") != 0 && strcmp(args->xml_type, "PROJECT") != 0) {
                fprintf(stderr, "[Error:%s] the xml_type (%s) is INVALID!\n\n", __func__, optarg);
                exit(-1);
            }
            break;

        case 'd':
            args->database = params_str_dup(optarg);
            break;

        default:
            args->help = 1;
            params_show_usage(PARAMS_BUILD);
            break;
        }
    }

    /* check the required parameters */
    if (!args->xml_file || !args->database) {
        fprintf(stderr, "[Error:%s] the xml file and database is required!\n\n", __func__);
        params_show_usage(PARAMS_BUILD);
    }

    return args;
}


static const struct option sample_options[] =
{
    {"help",  no_argument,  NULL, 'h'},
    {"xml_file", required_argument,  NULL, 'f'},
    {"xml_date",  required_argument,  NULL, 'e'},
    {"database",  required_argument,  NULL, 'd'},
    {"output_dir",  required_argument,  NULL, 'o'},
    {NULL,  0,  NULL,  0}
};


static args_t *params_sample_parse(int argc, char **argv)
{
    int opt;
    args_t *args;

    /* set the default parameters */
    err_calloc(args, 1, args_t);
    args->params_mode = PARAMS_SAMPLE;

    /* parse the command line parameters */
    while ( (opt = getopt_long(argc, argv, "f:e:d:o:h", sample_options, NULL)) != -1 )
    {
        switch (opt) {
            case 'h':
                args->help = 1;
                params_show_usage(PARAMS_SAMPLE);
                break;

            case 'f':
                args->xml_file = params_str_dup(optarg);
                break;

            case 'e':
                args->xml_date = (int)strtol(optarg, NULL, 10);
                if (args->xml_date < 20250101 || args->xml_date > 20990101) {
                    fprintf(stderr, "[Error:%s] the xml date (%s) is INVALID!\n\n", __func__, optarg);
                    exit(-1);
                }
                break;

            case 'd':
                args->database = params_str_dup(optarg);
                break;

            case 'o':
                args->output_dir = params_str_dup(optarg);
                break;

            default:
                args->help = 1;
                params_show_usage(PARAMS_SAMPLE);
                break;
        }
    }

    /* check the required parameters */
    if (!args->xml_file || !args->database || !args->output_dir) {
        fprintf(stderr, "[Error:%s] the xml file, database and output directory are required!\n\n", __func__);
        params_show_usage(PARAMS_SAMPLE);
    }

    return args;
}



static const struct option project_options[] =
{
    {"help",  no_argument,  NULL, 'h'},
    {"xml_file", required_argument,  NULL, 'f'},
    {"xml_date",  required_argument,  NULL, 'e'},
    {"database",  required_argument,  NULL, 'd'},
    {"output_dir",  required_argument,  NULL, 'o'},
    {NULL,  0,  NULL,  0}
};


static args_t *params_project_parse(int argc, char **argv)
{
    int opt;
    args_t *args;

    /* set the default parameters */
    err_calloc(args, 1, args_t);
    args->params_mode = PARAMS_PROJECT;

    /* parse the command line parameters */
    while ( (opt = getopt_long(argc, argv, "f:e:d:o:h", project_options, NULL)) != -1 )
    {
        switch (opt) {
            case 'h':
                args->help = 1;
                params_show_usage(PARAMS_PROJECT);
                break;

            case 'f':
                args->xml_file = params_str_dup(optarg);
                break;

            case 'e':
                args->xml_date = (int)strtol(optarg, NULL, 10);
                if (args->xml_date < 20250101 || args->xml_date > 20990101) {
                    fprintf(stderr, "[Error:%s] the xml date (%s) is INVALID!\n\n", __func__, optarg);
                    exit(-1);
                }
                break;

            case 'd':
                args->database = params_str_dup(optarg);
                break;

            case 'o':
                args->output_dir = params_str_dup(optarg);
                break;

            default:
                args->help = 1;
                params_show_usage(PARAMS_PROJECT);
                break;
        }
    }

    /* check the required parameters */
    if (!args->xml_file || !args->database || !args->output_dir) {
        fprintf(stderr, "[Error:%s] the xml file, database and output directory are required!\n\n", __func__);
        params_show_usage(PARAMS_PROJECT);
    }

    return args;
}


args_t *params_parse(int argc, char **argv)
{
    args_t *args = NULL;

    if (argc < 2) {  /* not enough parameters */
        params_show_usage(PARAMS_INVALID);
        return args;
    }

    /* check the first parameter to decide which function to call */
    if (strcmp(argv[1], "build") == 0)
        args = params_build_parse(argc-1, argv+1);

    else if (strcmp(argv[1], "sample") == 0)
        args = params_sample_parse(argc-1, argv+1);

    else if (strcmp(argv[1], "project") == 0)
        args = params_project_parse(argc-1, argv+1);

    else {
        fprintf(stderr, "[Error:%s] unrecognized command '%s' is detected!\n\n", __func__, argv[1]);
        params_show_usage(PARAMS_INVALID);
    }

    return args;
}
