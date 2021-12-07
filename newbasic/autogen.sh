#!/bin/sh
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(cd $srcdir; libtoolize --force; \
aclocal; automake -a --add-missing; autoconf)

$srcdir/configure "$@"


