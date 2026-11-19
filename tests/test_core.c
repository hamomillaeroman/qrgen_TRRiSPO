#include "core.h"
#include <stdio.h>
#include <unistd.h>

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", msg); failures++; } } while (0)

int main(void)
{
    const char *tmp = "test_qr.png";
    FILE *f = fopen(tmp, "wb");

    CHECK(qr_generate_png("x", f, 100, 5) != 0, "bad level rejected");
    CHECK(qr_generate_png("", f, 100, 1) != 0, "empty text rejected");
    for (int l = 0; l <= 3; l++) {
        CHECK(qr_generate_png("hello", f, 64, l) == 0, "valid level works");
    }
    CHECK(qr_generate_png("a", f, 1, 1) == 0, "small size clamped");
    CHECK(qr_generate_png(NULL, f, 100, 1) != 0, "NULL text rejected");
    CHECK(qr_generate_png("x", NULL, 100, 1) != 0, "NULL stream rejected");
    CHECK(qr_level_valid(0) && qr_level_valid(3), "valid levels");
    CHECK(!qr_level_valid(-1) && !qr_level_valid(4), "invalid levels");

    fclose(f);
    unlink(tmp);
    return failures ? 1 : 0;
}
