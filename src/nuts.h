#ifndef __nuts_h
#define __nuts_h

#include <signal.h>
#include "termbox2.h"
#include "files.h"

#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

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
#define eof_padding 16
#define page_updn_lines 16

typedef struct {
    char* file_contents;
    long scroll_v_offset; // vertical offset into file buffer, what is skipped from rendering, in lines
    long scroll_h_offset; // horizontal offset
    // long pos;             // position in the buffer
    // long len;             // size of the buffer
    long line; // current line in the buffer
    long col;  // intended column (intended by the user, used when moving cursor)
    long cx;   // screen coordinates
    long cy;   // starts at 2 due to header
} editor_t;

typedef struct {
    long llen;
    bool eof;
} line_t;

extern editor_t ed;

editor_t ed_init(void);

size_t max_width(void);
size_t max_height(void);

line_t get_line_len(void);

void try_move_cursor_up(int lc); // line count, 1 or page_updn_lines
void try_move_cursor_down(int lc);
void try_move_cursor_left(void);
void try_move_cursor_right(void);

void move_start(void);
void move_end(void);

#endif
