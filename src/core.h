#ifndef QRGEN_CORE_H
#define QRGEN_CORE_H

#include <stdio.h>

int qr_generate_png(const char *text, FILE *out_stream, int size, int level);
int qr_level_valid(int level);

#endif
