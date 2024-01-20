#include "nuts.h"

editor_t ed_init(void) {
    return (editor_t){.cx = left_margin, .cy = size_header};
}

size_t max_width(void) {
    return tb_width() - right_margin;
}

size_t max_height(void) {
    return tb_height() - size_footer;
}

// TODO bad perf: should build this as an array of lines lenghts once
// TODO: handle \r\n
size_t get_line_len() {
    size_t line = 0;
    size_t len = 0;
    size_t i = 0;
    bool found = false;
    const char* file = ed.file_contents;

    while (i < strlen(file)) {
        if (file[i] == '\n') {
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

void try_move_cursor_up() {
    size_t ll;

    if (ed.cy <= size_header) {
        if (ed.scroll_v_offset > 0) {
            ed.scroll_v_offset--;
            ed.line--;
            assert(ed.line >= 0 && "line should not be negative");
        }
    } else {
        ed.cy--;
        ed.line--;
        assert(ed.line >= 0 && "line should not be negative");
    }
    ll = get_line_len();
    ed.cx = (ed.cx >= ll - 1) ? ll - 1 : (ed.cx); // if cursor is past end of line, move it back
    if (ed.col < ll)
        ed.cx = ed.col; // move the cursor to the user's intended column
}

void try_move_cursor_down() {
    int mh = max_height();
    size_t ll;

    if (ed.cy >= mh) {
        ed.scroll_v_offset++;
        ed.line++;
    } else {
        ed.cy++;
        ed.line++;
    }
    ll = get_line_len();
    ed.cx = (ed.cx >= ll - 1) ? ll - 1 : (ed.cx);
    if (ed.col < ll)
        ed.cx = ed.col;
}

void try_move_cursor_left() {
    ed.cx = (ed.cx > left_margin) ? (ed.cx - 1) : left_margin;
    ed.col = ed.col > left_margin ? (ed.col - 1) : left_margin;
}

void try_move_cursor_right() {
    size_t ll;

    ll = get_line_len();
    ed.cx = (ed.cx >= ll - 1) ? ll - 1 : (ed.cx + 1);
    ed.col = (ed.col >= ll - 1) ? ll - 1 : (ed.col + 1);
}
