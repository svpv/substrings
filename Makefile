CC = clang
CFLAGS = -O2 -march=native -mcmodel=medium -fvisibility=hidden -g -Wall
substrings: substrings.c lines.c lcp.c ldi.c sufbase.c sufsort.c sufmerge.c
