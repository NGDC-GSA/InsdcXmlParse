Insdc XML file parser
=========================
INSDC XML file parser for bioproject and biosample.

PROGRAM: InsdcXmlParser<br>
VERSION: 1.1.0<br>
PLATFORM: Linux<br>
COMPILER: >= gcc-4.8.5<br>
AUTHOR: xiaolong zhang<br>
EMAIL: zhangxiaolong@big.ac.cn<br>
DATE:   2025-11-27<br>
UPDATE: 2025-12-08<br>
DEPENDENCE:<br>
* GNU make and gcc<br>
* openmp library<br>

Description
=========================
* InsdcXmlParser is an efficient tool to detect differences between two XML files and output the XML segments that different. </br>
* The software provides three core functions: </br>
1. build (for constructing the database </br>
2. sample (for analyzing differences between sample XML file and the database) </br>
3. project (for analyzing differences between project XML file and the database) </br>



Building
=========================

```shell
cd InsdcXmlParser
make
```

Usage
========================

```shell
Program: xml_parser (v1.1.0)
CreateDate: 2025-11-27
UpdateDate: 2025-12-08
Author: XiaolongZhang (xiaolongzhang2015@163.com)

Usage: xml_parser <command> [options]

Commands:
    build          build database index
                   input: xml file of given release date
                   output: database file (.db)

    sample         parse and compare the difference between database and current xml file
                   input: biosample xml file (biosample_set.xml) and the database index
                   output: the different data body updated by INSDC

    project        parse and compare the difference between database and current xml file
                   input: bioproject xml file (bioproject.xml) and the database index
                   output: the different data body updated by INSDC
```

## 1. build
```shell
$ xml_parser build -h

Program: xml_parser (v1.1.0)
CreateDate: 2025-11-27
UpdateDate: 2025-12-08
Author: XiaolongZhang (xiaolongzhang2015@163.com)

Usage: xml_parser build [options]

Options:
    -h|--help                    show help information

[Required]
    -f|--xml_file      FILE      the xml file used to build the database
    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)
    -t|--xml_type      STRING    the type of xml file [SAMPLE|PROJECT]
    -d|--database      FILE      the output xml database file (.db)
```


## 2. sample
```shell
$ xml_parser sample -h

Program: xml_parser (v1.1.0)
CreateDate: 2025-11-27
UpdateDate: 2025-12-08
Author: XiaolongZhang (xiaolongzhang2015@163.com)

Usage: xml_parser sample [options]

Options:
    -h|--help                    show help information

[Required]
    -f|--xml_file      FILE      the sample xml file used to compare with the database
    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)
    -d|--database      FILE      the sample xml database file (.db)
    -o|--output_dir    STRING    the output directory
```

## 3. project

```shell
$ xml_parser project -h

Program: xml_parser (v1.1.0)
CreateDate: 2025-11-27
UpdateDate: 2025-12-08
Author: XiaolongZhang (xiaolongzhang2015@163.com)

Usage: xml_parser project [options]

Options:
    -h|--help                    show help information

[Required]
    -f|--xml_file      FILE      the project xml file used to compare with the database
    -e|--xml_date      INT       the released date of the xml file (e.g. 20251208)
    -d|--database      FILE      the project xml database file (.db)
    -o|--output_dir    STRING    the output directory
```

Example
==============

## 1. build the database
```shell
# run the build module
$ ./xml_parser build -f test/sample_set.xml -e 20251130 -t SAMPLE -d test/sample.db

# output information
[2025-12-9 9:52:12] start to build the database ...
[*] parse number of items: 449
[*] database version: SAMPLE (20251130)
[2025-12-9 9:52:12] done!

# database
The test/sample.db is the sample database of 20251130 
```

## 2. compare the sample xml
```shell
# run the sample module
./xml_parser sample -f test/current_set.xml -e 20251205 -d test/sample.db -o test/

# output information
[2025-12-9 9:55:15] start to load the database ...
[*] database version: SAMPLE (20251130)
[2025-12-9 9:55:15] done!
[2025-12-9 9:55:15] start to compare the difference ...
[*] compare number of items: 448
[2025-12-9 9:55:15] done!

# database
The test/sample.db is update to SAMPLE (20251205), same as the xml file provided

# difference between current xml (20251205) and the database (20251130)
1. sample_diff.list (difference of all items with format: 'Difference\tSampleId')
DELETE  2
DELETE  3
CHANGE  5
ADD     66666

2. sample_diff.xml (only CHANGE and ADD will be output)
Is a normal XML file
```

Performance
============
1. Build database with biosample of 20251130 (about 129GB)
* Total time consumed: 228 seconds
* Memory footprint: about 1.7GB
```shell
./xml_parser build -f ../../sample/20251130/biosample_set.xml -e 20251130 -t SAMPLE -d biosample.db

[2025-12-9 8:5:4] start to build the database ...
[*] parse number of items: 50069339
[*] database version: SAMPLE (20251130)
[2025-12-9 8:8:52] done!
```

2. Compare the difference with biosample of 20251205 (about 129GB)
* Total time consumed: 235 seconds
* Memory footprint: about 1.7GB

```shell
./xml_parser sample -f ../../sample/20251205/biosample_set.xml -e 20251205 -d biosample.db -o 20251130-20251205/

[2025-12-9 8:11:14] start to load the database ...
[*] database version: SAMPLE (20251130)
[2025-12-9 8:11:14] done!
[2025-12-9 8:11:14] start to compare the difference ...
[*] compare number of items: 50245012
[2025-12-9 8:15:9] done!
```
