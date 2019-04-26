#!/bin/sh

ssh minix << EOF
  cd ../usr/src/minix/servers/pm
  make
  make install
  cd ~
  cd ../usr/src/releasetools
  make do-hdboot
  cd ~
  cd ../usr/src/lib/libc
  make
  make install
  reboot
EOF