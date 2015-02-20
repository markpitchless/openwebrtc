#!/bin/sh
gcc tests/cocoon_client.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a openwebrtc-deps-x86_64-linux/lib/libsoup-2.4.a -o cocoon_client -I./out/x86_64-unknown-linux/include/owr -I ./openwebrtc-deps-x86_64-linux/include/glib-2.0/ -I ./openwebrtc-deps-x86_64-linux/include/json-glib-1.0/ -I ./openwebrtc-deps-x86_64-linux/include/libsoup-2.4/ -L ./out/x86_64-unknown-linux/./lib -lopenwebrtc

gcc -g -v -da -Q tests/cocoon_client_std.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a openwebrtc-deps-x86_64-linux/lib/libsoup-2.4.a -o cocoon_client_std -I./out/x86_64-unknown-linux/include/owr -I ./openwebrtc-deps-x86_64-linux/include/glib-2.0/ -I ./openwebrtc-deps-x86_64-linux/include/json-glib-1.0/ -I ./openwebrtc-deps-x86_64-linux/include/libsoup-2.4/ -L ./out/x86_64-unknown-linux/./lib -lopenwebrtc

gcc tests/cocoon_send_receive.c tests/test_utils.c tests/cocoon_utils.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a -o cocoon_send_receive -I./out/x86_64-unknown-linux/include/owr -I ./openwebrtc-deps-x86_64-linux/include/glib-2.0/ -L ./out/x86_64-unknown-linux/lib -lopenwebrtc

gcc tests/cocoon_send.c tests/test_utils.c tests/cocoon_utils.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a -o cocoon_send -I./out/x86_64-unknown-linux/include/owr -I ./openwebrtc-deps-x86_64-linux/include/glib-2.0/ -L ./out/x86_64-unknown-linux/lib -lopenwebrtc

gcc tests/cocoon_receive.c tests/test_utils.c tests/cocoon_utils.c openwebrtc-deps-x86_64-linux/lib/libjson-glib-1.0.a -o cocoon_receive -I./out/x86_64-unknown-linux/include/owr -I ./openwebrtc-deps-x86_64-linux/include/glib-2.0/ -L ./out/x86_64-unknown-linux/lib -lopenwebrtc
# Needs
# export LD_LIBRARY_PATH=$PWD/out/x86_64-unknown-linux/lib
