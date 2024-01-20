#ifndef __nuts_h
#define __nuts_h

#include "termbox2.h"
#include "files.h"

#include <stdbool.h>
#include <assert.h>

#define ASCII_SINGLE_QUOTE 39
#define ASCII_DOUBLE_QUOTE 34
#define ASCII_LOWER_A 97
#define ASCII_LOWER_Z 122
#define ASCII_UPPER_A 65
#define ASCII_UPPER_Z 90
#define ASCII_0 48
#define ASCII_9 57

#define FG TB_WHITE
#define SCREEN_BG TB_DEFAULT
#define FRAME_BG (TB_BLACK | TB_BRIGHT)
#define size_header 2
#define size_footer 2
#define left_margin 0
#define right_margin 2

typedef struct editor_s editor_t;

struct editor_s {
    char* file_contents;
    size_t scroll_v_offset; // vertical offset into file buffer, what is skipped from rendering, in lines
    size_t scroll_h_offset; // horizontal offset
    size_t line;            // file buffer line
    size_t col;             // intended column (intended by the user, used when moving cursor)
    int cx;                 // screen coordinates
    int cy;                 // starts at 2 due to header
};

extern editor_t ed;

editor_t ed_init(void);

int max_width(void);
int max_height(void);

size_t get_line_len(void);
size_t get_line_num(void);
size_t get_line_num_len(size_t line);

void try_move_cursor_up(void);
void try_move_cursor_down(void);
void try_move_cursor_left(void);
void try_move_cursor_right(void);

#endif
