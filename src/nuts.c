#define TB_IMPL
#include "nuts.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

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
}

static editor_t ed = {0};

const char*
get_file(void) {
    if (ed.file_contents) {
        return ed.file_contents;
    } else {
        file_t res = read_file_content("/Users/n/Code/site-gen-c/src/settings.c");
        if (res.error) {
            fprintf(stderr, "could not load file...\n");
            exit(1);
        }
        ed.file_contents = res.data;
        return ed.file_contents;
    }
}

void draw_text_buffer(void) {
    // if not file, load file
    const char* file = get_file();
    // decide how many lines we are going to draw
    int draw_line_count = tb_height() - size_header - size_footer - 4;
    // decide the offset from the top

    size_t i = 0;
    char ch;
    int x = 0, y = 0;

    do {
        ch = file[i];
        if (ch == '\n') {
            x = 0;
            y++;
            if (y > draw_line_count)
                break;
            continue;
        }
        tb_set_cell((x++) + left_margin + ed.scroll_h_offset, y + size_header + ed.scroll_v_offset, ch, FG, BG);
    } while (file[++i] != '\0');

    // char* lines[draw_line_count] = file_split_lines(file, offset);

    // for each line
    // 		draw every cell?
}

void render(void) {
    print_word(10, 10, "Hello, world!");
    print_word(10, 11, "Press CTRL Q to terminate.");
    draw_background();
    draw_frame();
    draw_text_buffer();
    tb_set_cursor(10, 10);
    tb_present();
}

void handle_key(struct tb_event ev) {
}

int main(void) {
    int ret = tb_init();

    if (ret) {
        fprintf(stderr, "Could not even TUI. RIP.");
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
                    if (ev.key == TB_KEY_CTRL_Q)
                        goto RIP;
                    handle_key(ev);
                    break;
                case (TB_EVENT_MOUSE):
                    // todo handle mouse events
                    break;
                case (TB_EVENT_RESIZE):
                    render();
                    break;
            }
            tb_present();

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
