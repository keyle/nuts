#include "nuts.h"

void ed_debug() {
    FILE* f = fopen("debug", "w");
    assert(f != NULL && "could not debug contents");

    for (size_t i = 0; i < ed.contents.cap; i++) {
        char c = ed.contents.data[i];
        if (c == '\0')
            c = '%';
        fprintf(f, "%c", c);
    }

    fprintf(f, "\n\n\n");

    fclose(f);
}
