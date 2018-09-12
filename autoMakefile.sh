#!/bin/bash

src=$(ls *.c)
cc=gcc
bin=httpd
cat <<EOF >Makefile
LDFLAGS=-lpthread
$bin:$src
	$cc -o \$@ \$^ \$(LDFLAGS)
.PHONY:clean
clean:
	rm -f $bin
EOF
