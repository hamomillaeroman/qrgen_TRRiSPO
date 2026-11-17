#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

static void load_config(const char *path, int *level, int *size)
{
    FILE *f = fopen(path, "r");
    if (!f) return; /* отсутствие файла — не ошибка */

    char line[256];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char key[64];
        int val;
        if (sscanf(line, "%63[^=]=%d", key, &val) == 2) {
            if (strcmp(key, "level") == 0 && val >= 0 && val <= 3) *level = val;
            else if (strcmp(key, "size") == 0 && val > 0) *size = val;
        }
    }
    fclose(f);
}

static void usage(const char *prog)
{
    printf("Usage: %s [options]\n", prog);
    printf("  -t, --text <string>    текст для кодирования (обязательно)\n");
    printf("  -o, --output <file>    выходной PNG (по умолчанию qr.png)\n");
    printf("  -s, --size <px>        размер PNG\n");
    printf("  -l, --level <0-3>      коррекция: 0=L, 1=M, 2=Q, 3=H\n");
    printf("  -h, --help             справка\n");
    printf("  -V, --version          версия\n");
}

int main(int argc, char **argv)
{
    const char *text = NULL;
    const char *output = "qr.png";
    int size = 256;
    int level = 1;

    load_config("/etc/qrgen/qrgen.conf", &level, &size);

    static struct option long_opts[] = {
        {"text", required_argument, 0, 't'}, {"output", required_argument, 0, 'o'},
        {"size", required_argument, 0, 's'}, {"level", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},       {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "t:o:s:l:hV", long_opts, NULL)) != -1) {
        switch (opt) {
        case 't': text = optarg; break;
        case 'o': output = optarg; break;
        case 's': size = atoi(optarg); break;
        case 'l': level = atoi(optarg); break;
        case 'V': printf("qrgen %s\n", APP_VERSION); return 0;
        case 'h': usage(argv[0]); return 0;
        default: usage(argv[0]); return 1;
        }
    }

    if (!text) {
        fprintf(stderr, "Ошибка: не указан текст (--text).\n");
        return 1;
    }
    if (!qr_level_valid(level)) {
        fprintf(stderr, "Ошибка: уровень коррекции 0..3.\n");
        return 1;
    }
    if (qr_generate_png(text, output, size, level) != 0) {
        fprintf(stderr, "Ошибка: не удалось создать PNG.\n");
        return 1;
    }
    return 0;
}
