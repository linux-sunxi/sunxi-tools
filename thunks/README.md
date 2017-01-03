
# thunks/README.md

This directory contains assembly sources for ARM [thunk] code, and
a corresponding _Makefile_. The idea is that the resulting binary routines
can be transferred to a suitable target device and then executed 'remotely',
usually via `sunxi-fel`.

Normally you don't need to change or (re)build anything within this folder.
Currently our main build process (via the parent directory's _Makefile_)
only includes `fel-to-spl-thunk.h` directly. Other _.h_ files are provided
**just for reference**. The main purpose of this folder is simply keeping
track of _.S_ sources, to help with possible future maintenance of the
various code snippets.

Please note that any files lacking explicit license information are intended
to be covered by the project's [overall license](../LICENSE.md) (GPLv2).


[thunk]: https://en.wikipedia.org/wiki/Thunk#Interoperability
