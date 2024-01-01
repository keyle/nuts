#ifndef __nuts_h
#define __nuts_h

#include "termbox2.h"
#include "files.h"

#define FG TB_WHITE
#define BG TB_BLUE
#define size_header 2
#define size_footer 2
#define left_margin 2
#define right_margin 2

typedef struct editor_s editor_t;

struct editor_s {
    char* file_contents;
    size_t scroll_v_offset;
    size_t scroll_h_offset;
    int cx;
    int cy;
};

editor_t ed_init(void);

#endif
