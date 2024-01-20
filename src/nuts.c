#define TB_IMPL
#include "nuts.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

editor_t ed = {0};

void print_word(int x, int y, const char text[static 1]) {
    int i = 0;
    while (text[i] != '\0') {
        tb_set_cell(x + i, y, text[i], FG, SCREEN_BG);
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
            tb_set_cell(i, j, 0, 0, SCREEN_BG);
        }
    }
}

void draw_frame(void) {
    int width = tb_width();
    int height = tb_height();
    int i;

    for (i = 0; i < height; i++) {
        if (left_margin > 0) {
            tb_set_cell(0, i, 0, 0, SCREEN_BG);
        }
        tb_set_cell(width - 1, i, 0, 0, SCREEN_BG);
    }

    for (i = 0; i < width; i++) {
        tb_set_cell(i, 0, 0, 0, FRAME_BG);
        tb_set_cell(i, 1, 0, 0, SCREEN_BG);
        tb_set_cell(i, height - 1, 0, 0, FRAME_BG);
    }

    draw_word("F", 2, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("ILE", 3, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    draw_word("E", 10, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("DIT", 11, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    draw_word("S", 19, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("EARCH", 20, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    draw_word("R", 30, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("UN", 31, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    draw_word("B", 38, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("UILD", 39, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    draw_word("D", 48, 0, TB_RED | TB_BOLD, FRAME_BG);
    draw_word("EBUG", 49, 0, TB_BLACK | TB_BOLD, FRAME_BG);

    /********/

    tb_printf(width - 10, height - 1, TB_RED | TB_BOLD, FRAME_BG, "F12");
    tb_printf(width - 6, height - 1, TB_BLACK | TB_BOLD, FRAME_BG, "QUIT");
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

void draw_text_buffer(void) {
    const char* file = get_buffer_contents();
    int draw_line_count = max_height() - size_header;

    size_t i = 0;
    char ch;
    int x = 0, y = 0;
    uint16_t color;

    bool in_dbl_quotes = false;
    bool in_sgl_quotes = false;

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

        tb_set_cell((x++) + left_margin - ed.scroll_h_offset, y + size_header - ed.scroll_v_offset, ch, color, SCREEN_BG);

    } while (file[++i] != '\0');
}

void print_status_bar(void) {
    int height = tb_height();
    char temp[130];
    size_t ll = get_line_len();

    // debug temporary print
    sprintf(temp, "(%zu,%zu) cx %zu cy %zu | line %zu | ll %zu", (ed.cy - size_header + 1), (ed.cx - left_margin + 1), ed.cx, ed.cy, ed.line, ll);
    tb_printf(2, height - 1, TB_BLACK | TB_BOLD | TB_ITALIC, FRAME_BG, temp);
}

void render(void) {
    draw_background();
    draw_text_buffer();
    draw_frame();
    tb_set_cursor(ed.cx, ed.cy);
    print_status_bar();
    tb_present();
}

void handle_key(struct tb_event ev) {
    switch (ev.key) {
        case TB_KEY_ARROW_UP:
            try_move_cursor_up();
            break;

        case TB_KEY_ARROW_DOWN:
            try_move_cursor_down();
            break;

        case TB_KEY_ARROW_LEFT:
            try_move_cursor_left();
            break;

        case TB_KEY_ARROW_RIGHT:
            try_move_cursor_right();
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
