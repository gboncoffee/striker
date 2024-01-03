#!/usr/bin/sh

set -xe

CC=gcc
command -v tcc > /dev/null && CC=tcc

CLIBS="-lX11 -lGL -lm -lpthread -ldl -lrt libraylib.a"
CFLAGS="-Wall -Wextra -pedantic -std=c99 -g"

$CC striker.c $CLIBS $CFLAGS -o striker
