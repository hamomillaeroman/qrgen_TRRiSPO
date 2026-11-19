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
    if (!f) return; 
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
    printf("Usage: %s <command> [options]\n", prog);
    printf("Commands:\n  generate               Сгенерировать QR-код в PNG\n\n");
    printf("Options for generate:\n");
    printf("  -t, --text <string>    Текст для кодирования (обязательно)\n");
    printf("  -o, --output <file>    Выходной PNG (по умолчанию qr.png)\n");
    printf("  -s, --size <px>        Размер PNG\n");
    printf("  -l, --level <0-3>      Коррекция: 0=L, 1=M, 2=Q, 3=H\n\n");
    printf("Global options:\n  -h, --help             Справка\n  -V, --version          Версия\n");
}

int main(int argc, char **argv)
{
    const char *text = NULL;
    const char *output = "qr.png";
    int size = 256;
    int level = 1;

    if (argc < 2) { usage(argv[0]); return 1; }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) { usage(argv[0]); return 0; }
    if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0) { printf("qrgen %s\n", APP_VERSION); return 0; }

    if (strcmp(argv[1], "generate") != 0) {
        fprintf(stderr, "Ошибка: неизвестная команда '%s'.\n", argv[1]);
        return 1;
    }

    load_config("/etc/qrgen/qrgen.conf", &level, &size);

    optind = 2;
    static struct option long_opts[] = {
        {"text", required_argument, 0, 't'}, {"output", required_argument, 0, 'o'},
        {"size", required_argument, 0, 's'}, {"level", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "t:o:s:l:", long_opts, NULL)) != -1) {
        switch (opt) {
        case 't': text = optarg; break;
        case 'o': output = optarg; break;
        case 's': size = atoi(optarg); break;
        case 'l': level = atoi(optarg); break;
        default: return 1;
        }
    }
if (!text) {
        fprintf(stderr, "Ошибка: не указан текст (--text).\n");
        return 1;
    }
    if (!qr_level_valid(level)) {
        fprintf(stderr, "Ошибка: уровень коррекции должен быть от 0 до 3.\n");
        return 1;
    }

    FILE *fp = fopen(output, "wb");
    if (!fp) {
        fprintf(stderr, "Ошибка: не удалось открыть файл '%s'.\n", output);
        return 1;
    }

    int rc = qr_generate_png(text, fp, size, level);
    fclose(fp);

    if (rc != 0) {
        fprintf(stderr, "Ошибка: не удалось сгенерировать QR-код.\n");
        return 1;
    }
    return 0;
}
