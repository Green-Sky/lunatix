FROM ubuntu:20.04

RUN apt-get update && \
 DEBIAN_FRONTEND="noninteractive" apt-get install -y --no-install-recommends \
 libc-dev \
 libopus-dev \
 libsodium-dev \
 libvpx-dev \
 pkg-config \
 tcc \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
COPY auto_tests/ /work/auto_tests/
COPY testing/ /work/testing/
COPY toxav/ /work/toxav/
COPY toxcore/ /work/toxcore/
COPY toxencryptsave/ /work/toxencryptsave/

RUN tcc \
 -Dinline=static \
 -o send_message_test \
 -Wall -Werror \
 -bench -g \
 auto_tests/auto_test_support.c \
 auto_tests/send_message_test.c \
 testing/misc_tools.c \
 toxav/*.c \
 toxcore/*.c \
 toxcore/*/*.c \
 toxencryptsave/*.c \
 third_party/cmp/*.c \
 $(pkg-config --cflags --libs libsodium opus vpx)

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
COPY other/make_single_file /work/other/
RUN \
 other/make_single_file \
   auto_tests/auto_test_support.c \
   auto_tests/send_message_test.c \
   testing/misc_tools.c | \
 tcc - \
   -o send_message_test \
   -Wall -Werror \
   -bench -g \
   $(pkg-config --cflags --libs libsodium opus vpx)
