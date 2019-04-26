#!/bin/sh

cd /usr/src/minix/servers/pm
make clean
make
make install
cd /usr/src/releasetools
make do-hdboot
cd /usr/src/lib/libc
rm distort_time.o distort_time.pico
make
make install
reboot