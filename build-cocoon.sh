#!/bin/sh
gcc tests/cocoon_client.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a openwebrtc-deps-x86_64-linux/lib/libsoup-2.4.a -o cocoon_client -I./out/x86_64-unknown-linux/include/owr -I ~/.openwebrtc/include/glib-2.0/ -I ~/.openwebrtc/lib/glib-2.0/include/ -I ./openwebrtc-deps-x86_64-linux/include/json-glib-1.0/ -I ./openwebrtc-deps-x86_64-linux/include/libsoup-2.4/ -L ./out/x86_64-unknown-linux/./lib -lopenwebrtc

# Needs
# export LD_LIBRARY_PATH=$PWD/out/x86_64-unknown-linux/lib
