.PHONY: clean
CC = gcc
CFLAGS = -std=c99 -fopenmp
LIBS =
XML_PARSER = xml_parser

DEBUG = 1

ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0 # enable debugging
else
	CFLAGS += -O2
endif


OBJECT = utils.o md5.o stream_reader.o xml_parser.o

all: $(XML_PARSER)

$(XML_PARSER): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJECT) $(XML_PARSER)

