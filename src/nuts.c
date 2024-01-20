#define TB_IMPL
#include "nuts.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>
#include <assert.h>

static editor_t ed = {0};

void print_word(int x, int y, const char text[static 1]) {
    int i = 0;
    while (text[i] != '\0') {
        tb_set_cell(x + i, y, text[i], FG, BG);
        i++;
    }
}

void draw_word(const char* word, int x, int y, uint16_t fg, uint16_t bg) {
    int i = 0;
    do {
        tb_set_cell(x + i, y, word[i], fg, bg);
    } while (word[++i] != '\0');
}

void draw_background(void) {
    int width = tb_width();
    int height = tb_height();
    int i, j;

    for (i = 0; i < width - 0; i++) {
        for (j = 0; j < height - 0; j++) {
            tb_set_cell(i, j, 0, 0, BG);
        }
    }
}

void draw_frame(void) {
    int width = tb_width();
    int height = tb_height();
    int i;

    for (i = 0; i < width; i++) {
        tb_set_cell(i, 0, 0, 0, TB_WHITE);
        tb_set_cell(i, height - 1, 0, 0, TB_WHITE);
    }

    for (i = 0; i < height; i++) {
        tb_set_cell(0, i, 0, 0, TB_WHITE);
        tb_set_cell(width - 1, i, 0, 0, TB_WHITE);
    }

    draw_word("F", 2, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("ILE", 3, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    draw_word("E", 10, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("DIT", 11, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    draw_word("S", 19, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("EARCH", 20, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    draw_word("R", 30, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("UN", 31, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    draw_word("B", 38, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("UILD", 39, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    draw_word("D", 48, 0, TB_RED | TB_BOLD, TB_WHITE);
    draw_word("EBUG", 49, 0, TB_BLACK | TB_BOLD, TB_WHITE);

    /********/

    tb_printf(width - 10, height - 1, TB_RED | TB_BOLD, TB_WHITE, "F12");
    tb_printf(width - 6, height - 1, TB_BLACK | TB_BOLD, TB_WHITE, "QUIT");

    // temporary print
    char temp[120];
    size_t ll = get_line_len(ed);
    sprintf(temp, "line %zu | ll %zu", ed.line, ll);

    tb_printf(2, height - 1, TB_BLUE | TB_BOLD, TB_WHITE, temp);
}

const char* get_buffer_contents(void) {
    if (ed.file_contents) {
        return ed.file_contents;
    } else {
        const char* file = "/Users/n/Code/site-gen-c/src/settings.c";
        file_t res = read_file_content(file);
        if (res.error) {
            tb_shutdown();
            fprintf(stderr, "Could not load file... %s\n", file);
            exit(1);
        }
        ed.file_contents = res.data;
        return ed.file_contents;
    }
}

int max_width(void) {
    return tb_width() - right_margin;
}

int max_height(void) {
    return tb_height() - size_footer;
}

void draw_text_buffer(void) {
    const char* file = get_buffer_contents();
    int draw_line_count = max_height() - size_header;

    size_t i = 0;
    char ch;
    int x = 0, y = 0;
    uint16_t color;

    bool in_dbl_quotes = false;
    bool in_sgl_quotes = false;

#define ASCII_SINGLE_QUOTE 39
#define ASCII_DOUBLE_QUOTE 34
#define ASCII_LOWER_A 97
#define ASCII_LOWER_Z 122
#define ASCII_UPPER_A 65
#define ASCII_UPPER_Z 90
#define ASCII_0 48
#define ASCII_9 57

    do {
        color = TB_CYAN | TB_BOLD;
        ch = file[i];
        if (ch == '\n') {
            x = 0;
            y++;
            if (y > draw_line_count)
                break;
            continue;
        }

        // flipping bits
        if (ch == ASCII_SINGLE_QUOTE)
            in_sgl_quotes = !in_sgl_quotes;
        else if (ch == ASCII_DOUBLE_QUOTE)
            in_dbl_quotes = !in_dbl_quotes;

        // colouring
        if (in_dbl_quotes || in_sgl_quotes || ch == ASCII_SINGLE_QUOTE || ch == ASCII_DOUBLE_QUOTE)
            color = TB_GREEN | TB_BOLD; // strings
        else if ((ch >= ASCII_UPPER_A && ch <= ASCII_UPPER_Z) || (ch >= ASCII_LOWER_A && ch <= ASCII_LOWER_Z))
            color = FG; // a-Z
        else if (ch >= ASCII_0 && ch <= ASCII_9)
            color = TB_GREEN | TB_BOLD; // numbers

        tb_set_cell((x++) + left_margin - ed.scroll_h_offset, y + size_header - ed.scroll_v_offset, ch, color, BG);
    } while (file[++i] != '\0');
}

void render(void) {
    print_word(10, 10, "Hello, world!");
    print_word(10, 11, "Press CTRL Q to terminate.");
    draw_background();
    draw_text_buffer();
    draw_frame();
    tb_set_cursor(ed.cx, ed.cy);
    tb_present();
}

void handle_key(struct tb_event ev) {
    int mh = max_height();
    int mw = max_width();
    size_t ll;
    switch (ev.key) {
        case TB_KEY_ARROW_UP:
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
            break;

        case TB_KEY_ARROW_DOWN:
            if (ed.cy >= mh) {
                ed.scroll_v_offset++;
                ed.line++;
            } else {
                ed.cy++;
                ed.line++;
            }
            break;

        case TB_KEY_ARROW_LEFT:
            ed.cx = ed.cx > left_margin ? ed.cx - 1 : left_margin;
            break;

        case TB_KEY_ARROW_RIGHT:
            ll = get_line_len(ed);
            ed.cx = ed.cx >= (ll + 1) ? (ll + 1) : ed.cx + 1;
            break;
    }
}

int main(void) {
    int ret = tb_init();
    ed = ed_init();

    if (ret) {
        fprintf(stderr, "Could not even TUI, bro.");
        exit(1);
    }

    render();

    struct tb_event ev;
    int res;

    while (1) {
        res = tb_poll_event(&ev);

        if (res == TB_OK) {
            switch (ev.type) {
                case (TB_EVENT_KEY):
                    if (ev.key == TB_KEY_CTRL_Q || ev.key == TB_KEY_F12)
                        goto RIP;
                    handle_key(ev);
                    break;
                case (TB_EVENT_MOUSE):
                    // todo handle mouse events
                    break;
                case (TB_EVENT_RESIZE):
                    break;
            }
            render();

        } else if (res == TB_ERR_POLL && tb_last_errno() == EINTR) {
            continue;
        } else if (res != TB_ERR_NO_EVENT) {
            fprintf(stderr, "(aborting) renderer error: %s\n", tb_strerror(res));
            return 1;
        }
    }

RIP:
    tb_shutdown();
    return 0;
}
