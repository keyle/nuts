#pragma once

#include <signal.h>
#include "termbox2.h"
#include "files.h"

#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h> // variadic func params

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
#define size_footer 1
#define left_margin 0
#define right_margin 2
#define eof_padding 16
#define page_updn_lines 16
#define FILE_PATH_MAX_LEN 4096 + 255
#define PARAM_MAX_LEN 1024 + 255
#define HOME_END_EOF 0 // HOME/END go to EOF or EOL

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct {
    file_t contents;
    long scroll_v_offset; // vertical offset into file buffer, what is skipped from rendering, in lines
    long scroll_h_offset; // horizontal offset
    long line;            // intended user line in the buffer
    long col;             // intended user column (when moving cursor)
    long cx;              // screen coordinates X
    long cy;              // Y starts at 2 due to header
    char file_path[FILE_PATH_MAX_LEN];
    char file_param[PARAM_MAX_LEN];
    char status[150]; // user friendly status at the bottom

} editor_t;

typedef struct {
    long llen;
    bool eof;
} line_t;

extern editor_t ed;

editor_t ed_init(void);

size_t max_width(void);
size_t max_height(void);

// utils
void status_write(const char *format, ...);

void handle_key(struct tb_event ev);

line_t get_line_len(void);

void try_move_cursor_up(int lc); // line count, 1 or page_updn_lines
void try_move_cursor_down(int lc);
void try_move_cursor_left(void);
void try_move_cursor_right(void);

void insert_ch(char ch);
void enter_ch(void);
void tab_ch(void);
void delete_ch(int offset);
void backspace_ch(void);

void move_start(void);
void move_end(void);

void func_save(void);
