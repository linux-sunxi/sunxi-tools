#!/bin/sh

aclocal
autoconf
libtoolize --force --copy
automake --force --copy --add-missing
