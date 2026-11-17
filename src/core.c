#include "core.h"
#include <qrencode.h>
#include <png.h>
#include <stdlib.h>

int qr_level_valid(int level)
{
    return level >= 0 && level <= 3;
}

static QRecLevel to_qr_level(int level)
{
    switch (level) {
    case 1: return QR_ECLEVEL_M;
    case 2: return QR_ECLEVEL_Q;
    case 3: return QR_ECLEVEL_H;
    default: return QR_ECLEVEL_L;
    }
}

static int write_png(const QRcode *qr, FILE *out_stream, int desired_size)
{
    const int border = 4;
    int modules = qr->width + 2 * border;
    int scale = (desired_size + modules - 1) / modules;
    if (scale < 1) scale = 1;
    int img_size = modules * scale;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return -1;
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        return -1;
    }
    
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return -1;
    }

    png_init_io(png, out_stream);
    png_set_IHDR(png, info, (png_uint_32)img_size, (png_uint_32)img_size, 8,
                 PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep row = malloc((size_t)img_size);
    if (!row) {
        png_destroy_write_struct(&png, &info);
        return -1;
    }

    for (int y = 0; y < img_size; y++) {
        int my = y / scale - border;
        for (int x = 0; x < img_size; x++) {
            int mx = x / scale - border;
            int black = 0;
            if (mx >= 0 && mx < qr->width && my >= 0 && my < qr->width) {
                black = qr->data[my * qr->width + mx] & 1;
            }
            row[x] = black ? 0 : 255;
        }
        png_write_row(png, row);
    }

    free(row);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    return 0;
}

int qr_generate_png(const char *text, FILE *out_stream, int size, int level)
{
    if (!text || !out_stream || !qr_level_valid(level)) return -1;
    if (size < 21) size = 256;

    QRcode *qr = QRcode_encodeString(text, 0, to_qr_level(level), QR_MODE_8, 1);
    if (!qr) return -1;

    int rc = write_png(qr, out_stream, size);
    QRcode_free(qr);
    return rc;
}
