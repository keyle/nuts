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
line_t get_line_len() {
    const char* file = ed.file_contents;
    bool found = false;
    size_t line = 0;
    size_t len = 0;
    size_t i = 0;

    while (i < strlen(file)) {
        if (file[i] == '\n') {
            if (found) {
                // printf("len %zd ", len);
                return (line_t){.llen = len, .eof = false};
            }
            len = -1;
            line++;
        }
        if (line == ed.line) {
            found = true;
        }
        len++;
        i++;
    }
    // printf("len %d ", 0);
    return (line_t){.llen = len, .eof = true};
}

// called when moving up and down
void resume_cx_desired_col() {
    line_t l = get_line_len();
    ed.cx = ed.col;                               // move the cursor to the user's intended column
    ed.cx = (ed.cx >= l.llen) ? l.llen : (ed.cx); // if cursor is past end of line, move it back
}

void try_move_cursor_up() {
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

    resume_cx_desired_col();
}

void try_move_cursor_down() {
    size_t mh = max_height();

    if (ed.cy >= mh) {
        ed.scroll_v_offset++;
        ed.line++;
    } else {
        ed.cy++;
        ed.line++;
    }

    resume_cx_desired_col();
}

// Move left, if it's the first char, and if we're not on the first line:
// move up and place the cursor at the end of the line
void try_move_cursor_left() {
    if (ed.cx <= left_margin) {
        if (ed.line == 0)
            return;
        try_move_cursor_up();
        ed.col = max_width();
        resume_cx_desired_col();
    } else {
        ed.cx--;
        ed.col = ed.cx;
    }
}

// Move right, if it's past the EOL, and we're not at the last line:
// move down and place the cursor on the first character
// FIXME the first line has an extra char space
void try_move_cursor_right() {
    line_t l = get_line_len();
    if (ed.cx >= l.llen) {
        try_move_cursor_down();
        ed.col = 0;
        resume_cx_desired_col();
    } else {
        ed.cx++;
        ed.col = ed.cx;
    }
}
