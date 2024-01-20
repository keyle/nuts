#ifndef __nuts_h
#define __nuts_h

#include "termbox2.h"
#include "files.h"

#include <stdbool.h>

#define FG TB_WHITE
#define BG TB_BLUE
#define size_header 2
#define size_footer 2
#define left_margin 2
#define right_margin 2

typedef struct editor_s editor_t;

struct editor_s {
    char* file_contents;
    size_t scroll_v_offset; // vertical offset into file buffer, what is skipped from rendering, in lines
    size_t scroll_h_offset; // horizontal offset
    size_t line;            // file buffer line
    int cx;                 // screen coordinates
    int cy;                 // starts at 2 due to header
};

editor_t ed_init(void);
size_t get_line_len(editor_t ed);

#endif
