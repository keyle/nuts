#include "nuts.h"

editor_t ed_init(void) {
    return (editor_t){.cx = left_margin, .cy = size_header};
}

size_t max_width(void) {
    return tb_width() - right_margin;
}

size_t max_height(void) {
    return tb_height() - (size_header + size_footer);
}

size_t max_line(void) {
    const char* file = ed.contents.data;
    size_t i = 0;
    size_t line = 0;

    while (i < strlen(file)) {
        if (file[i] == '\n')
            line++;
        i++;
    }

    return line;
}

// TODO: handle \r\n
line_t get_line_len() {
    const char* file = ed.contents.data;
    bool found = false;
    long line = 0;
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
        // go to the current editor line
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
    ed.cx = left_margin + ed.col;                                             // move the cursor to the user's intended column
    ed.cx = (ed.cx >= l.llen + left_margin) ? l.llen + left_margin : (ed.cx); // if cursor is past end of line, move it back
}

void move_start() {
    if (HOME_END_EOF) {
        ed.cx = left_margin;
        ed.cy = size_header; // 0 would be the screen to, we need the header offset
        ed.scroll_v_offset = 0;
        ed.line = 0;
        ed.col = 0;
    } else {
        ed.cx = left_margin;
        ed.col = 0;
    }
}

void try_move_cursor_up(int lc) {
    while (lc-- > 0) {
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
}

void try_move_cursor_down(int lc) {
    long mh = max_height();
    long ml = max_line();

    while (lc-- > 0) {
        if (ed.line >= ml + eof_padding)
            return;

        if (ed.cy >= mh) {
            ed.scroll_v_offset++;
        } else {
            ed.cy++;
        }
        ed.line++;
        resume_cx_desired_col();
    }
}

void move_end() {
    line_t line;
    if (HOME_END_EOF) {
        size_t mh = max_height();
        size_t ml = max_line();
        ed.line = ml;
        line = get_line_len();
        ed.col = line.llen;
        ed.scroll_v_offset = ml - mh + size_header + eof_padding;
        ed.cy = mh;
        resume_cx_desired_col();
    } else {
        line = get_line_len();
        ed.cx = left_margin + line.llen;
        ed.col = line.llen;
    }
}

// Move left, if it's the first char, and if we're not on the first line:
// move up and place the cursor at the end of the line
void try_move_cursor_left() {
    line_t line;
    if (ed.cx <= left_margin) {
        if (ed.line == 0)
            return;
        try_move_cursor_up(1);
        line = get_line_len();
        ed.col = line.llen;
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
        try_move_cursor_down(1);
        ed.col = 0;
        resume_cx_desired_col();
    } else {
        ed.cx++;
        ed.col = ed.cx;
    }
}

size_t pos_in_buffer(void) {
    size_t i = 0;
    long lc = ed.line;
    long cc = ed.col;

    while (i < ed.contents.size) {
        if (lc > 0) {
            if (ed.contents.data[i] == '\n') {
                lc--;
            }
            i++;
            continue;
        }
        if (cc > 0) {
            cc--;
            i++;
            continue;
        }
        return i;
    }

    return 0;
}

void insert_ch(char ch) {
    size_t i = pos_in_buffer();
    if (ed.contents.size >= ed.contents.cap) {
        // Reallocate to a larger size
        size_t new_cap = ed.contents.cap * 1.5;
        char* new_data = realloc(ed.contents.data, new_cap);
        if (!new_data) {
            perror("Failed to reallocate memory");
            return;
        }
        ed.contents.data = new_data;
        ed.contents.cap = new_cap;
    }

    // Shift characters right to make space for the new character
    for (size_t j = ed.contents.size; j > i; j--) {
        ed.contents.data[j] = ed.contents.data[j - 1];
    }

    // Insert the new character
    ed.contents.data[i] = ch;
    ed.contents.size++;
    try_move_cursor_right();
}

// TODO 'special' characters
void enter_ch(void) {
    insert_ch('\n');
}

void tab_ch(void) {
    insert_ch(' ');
    insert_ch(' ');
    insert_ch(' ');
    insert_ch(' ');
}

void delete_ch(int offset) {
    size_t i = MAX(0, pos_in_buffer() + offset);

    for (size_t j = i; j < ed.contents.size - 1; j++) {
        ed.contents.data[j] = ed.contents.data[j + 1];
    }

    ed.contents.size--;
}

void backspace_ch(void) {
    if (ed.cx > left_margin) {
        delete_ch(-1);
        try_move_cursor_left();
    } else {
        if (ed.line == 0)
            return;
        try_move_cursor_up(1);
        line_t l = get_line_len();
        try_move_cursor_down(1);
        delete_ch(-1);
        try_move_cursor_up(1);
        ed.col = l.llen;
        ed.cx = left_margin + l.llen;
    }
}
