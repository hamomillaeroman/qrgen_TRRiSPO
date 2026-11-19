#include "core.h"
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls)
{
    const char *text = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "text");
    if (!text) {
        const char *err = "Provide ?text=YourText in URL";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(err), (void*)err, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        MHD_destroy_response(response);
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

    struct MHD_Response *response = MHD_create_response_from_buffer(png_size, png_data, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "image/png");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int start_server(void)
{
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                                                 &answer_to_connection, NULL, MHD_OPTION_END);
    if (!daemon) return -1;
    printf("Server running on port %d\n", PORT);
    printf("Open: http://localhost:%d/?text=Hello\n", PORT);
    printf("Press Enter to stop...\n");
    getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
