srcdir = @srcdir@

lib_LTLIBRARIES = libccr.la

libccr_la_CFLAGS = -Wall -Wextra
libccr_la_CPPFLAGS = -I$(srcdir)/../../lib/libut/include -I$(srcdir)/../../cc
libccr_la_LIBADD = ../../cc/libcc.la ../../lib/libut_build/libut.la
libccr_la_SOURCES = ccr.c 
libccr_la_LDFLAGS = -version-info 0:0:0
include_HEADERS = ccr.h
