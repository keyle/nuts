#include "nuts.h"

editor_t ed_init(void) {
    return (editor_t){.cx = size_header, .cy = left_margin};
}

// TODO bad perf: should build this as an array of lines lenghts once
size_t get_line_len(editor_t ed) {
    size_t line = 0;
    size_t i = 0;
    size_t len = 0;
    bool found = false;
    const char* file = ed.file_contents;

    while (i < strlen(file)) {
        if (file[i] == '\n') { // TODO: handle \r\n
            if (found) {
                return len;
            }
            len = 0;
            line++;
        }
        if (line == ed.line) {
            found = true;
        }
        len++;
        i++;
    }
    return 0;
}
