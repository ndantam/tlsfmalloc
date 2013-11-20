ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/src

pkginclude_HEADERS = src/tlsf.h src/target.h

lib_LTLIBRARIES = libtlsf.la
libtlsf_la_SOURCES = src/tlsf.c src/debug.c src/bibop.c

lib_LTLIBRARIES += libtlsfmalloc.la
libtlsfmalloc_la_SOURCES = src/tlsf.c src/malloc.c src/debug.c src/bibop.c

noinst_PROGRAMS = test

test_SOURCES = test.c
test_LDADD = libtlsf.la
