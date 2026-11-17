#ifndef QRGEN_CORE_H
#define QRGEN_CORE_H

int qr_generate_png(const char *text, const char *out_path,
                    int size, int level);

int qr_level_valid(int level);

#endif
