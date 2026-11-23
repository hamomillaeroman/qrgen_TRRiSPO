#include "core.h"
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef APP_VERSION
#define APP_VERSION "1.0.0"
#endif

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls)
{
    (void)cls; (void)method; (void)version;
    (void)upload_data; (void)upload_data_size; (void)con_cls;

   // 1. Проверка живучести (Liveness)
    if (strcmp(url, "/health") == 0) {
        const char *resp = "OK";
        struct MHD_Response *r = MHD_create_response_from_buffer(strlen(resp), (void*)resp, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, r);
        MHD_destroy_response(r);
        return ret;
    }

    // 2. Проверка готовности (Readiness)
    if (strcmp(url, "/ready") == 0) {
        const char *resp = "READY";
        struct MHD_Response *r = MHD_create_response_from_buffer(strlen(resp), (void*)resp, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, r);
        MHD_destroy_response(r);
        return ret;
    }
// 3. Версия и имя пода (для проверки балансировки)
    if (strcmp(url, "/version") == 0) {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        char resp[512];
        snprintf(resp, sizeof(resp), "Version: %s | Pod: %s\n", APP_VERSION, hostname);
        struct MHD_Response *r = MHD_create_response_from_buffer(strlen(resp), (void*)resp, MHD_RESPMEM_MUST_COPY);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, r);
        MHD_destroy_response(r);
        return ret;
    }

    // 4. Генерация QR-кода (основная функция)
    const char *text = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "text");
    if (!text) {
        const char *err = "Provide ?text=YourText in URL";
        struct MHD_Response *r = MHD_create_response_from_buffer(strlen(err), (void*)err, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, r);
        MHD_destroy_response(r);
        return ret;
    }
char *png_data = NULL;
    size_t png_size = 0;
    FILE *stream = open_memstream(&png_data, &png_size);
    if (!stream) return MHD_NO;

    if (qr_generate_png(text, stream, 256, 1) != 0) {
        fclose(stream);
        free(png_data);
        return MHD_NO;
    }
    fclose(stream);

    struct MHD_Response *r = MHD_create_response_from_buffer(png_size, png_data, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(r, "Content-Type", "image/png");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, r);
    MHD_destroy_response(r);
    return ret;
}

int start_server(void)
{
    // Читаем порт из переменной окружения (передается из ConfigMap)
    const char *port_str = getenv("APP_PORT");
    int port = port_str ? atoi(port_str) : 8080;

    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, port, NULL, NULL,
                                                 (MHD_AccessHandlerCallback)answer_to_connection, NULL, MHD_OPTION_END);
    if (!daemon) return -1;
    printf("Server running on port %d\n", port);
    printf("Read APP_API_TOKEN from env: %s\n", getenv("APP_API_TOKEN") ? "YES (Secret works)" : "NO");
    
    while (1) {
        sleep(1);
    }
    
    MHD_stop_daemon(daemon);
    return 0;
}
