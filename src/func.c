#include "nuts.h"

// Editor functions

// TODO FIXME @next from an empty file, we write backwards!
// Saving works though. I think it's the \0 something
// also scrolling past end of line sort of works but remembers the last line's length
//      it's also confusing when saving because it looks like the file is longer than it is
// TODO saving should probably end the file with a \n

void func_save(void) {
    result_t res = write_file_content(ed.file_path, ed.contents.data);
    if (res.error) {
        tb_shutdown();
        perror("Could not func_save at write_file_contents");
        exit(1);
    }

    status_write("Saved %s", ed.file_path);
}
