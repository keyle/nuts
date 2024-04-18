#include "nuts.h"

// Editor functions

void func_save(void) {
    result_t res = write_file_content(ed.file_path, ed.contents.data);
    if (res.error) {
        tb_shutdown();
        perror("Could not func_save at write_file_contents");
        exit(1);
    }

    status_write("Saved %s", ed.file_path);
}
