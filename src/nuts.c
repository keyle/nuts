#define TB_IMPL
#include "nuts.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

editor_t ed = {0};
char* filepath;

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

    tb_printf(width - 10, height - 1, TB_RED | TB_BOLD, FRAME_BG, "F10");
    tb_printf(width - 6, height - 1, TB_BLACK | TB_BOLD, FRAME_BG, "QUIT");
}

const char* get_buffer_contents(void) {
    if (ed.contents.data) {
        return ed.contents.data;
    } else {
        file_t res = read_file_content(filepath);
        if (res.error) {
            tb_shutdown();
            fprintf(stderr, "Could not load file... %s\n", filepath);
            exit(1);
        }
        ed.contents = res;
        return ed.contents.data;
    }
}

uint16_t determine_color(char ch) {
    if (ch == '\'' || ch == '\"')
        return TB_GREEN | TB_BOLD; // strings
    else if (isalpha(ch))
        return FG; // a-Z
    else if (isdigit(ch))
        return TB_GREEN | TB_BOLD; // numbers
    return TB_CYAN | TB_BOLD;      // default
}

void draw_text_buffer(void) {
    const char* file = get_buffer_contents();
    size_t draw_line_count = max_height() - size_header;
    size_t skip_lines = ed.scroll_v_offset;
    uint16_t color;
    size_t i = 0, x = 0, y = 0;

    while (file[i] != '\0') {
        char ch = file[i];

        // Handle newline
        if (ch == '\n') {
            if (skip_lines > 0)
                skip_lines--;
            else {
                x = 0;
                y++;
            }
            if (y > draw_line_count + ed.scroll_v_offset)
                break;
            i++;
            continue;
        }

        if (skip_lines > 0) {
            i++;
            continue;
        }

        // Handle special characters (e.g., control characters)
        if (ch < ' ' || ch > '~') { // Non-printable characters
            i++;
            continue; // Skip rendering
        }
        // Set cell color based on content
        color = determine_color(ch);

        // Set character cell
        tb_set_cell(x + left_margin - ed.scroll_h_offset, y + size_header - ed.scroll_v_offset, ch, color, SCREEN_BG);
        x++;
        i++;
    }
}

void print_status_bar(void) {
    int height = tb_height();
    char temp[130];
    line_t l = get_line_len();

    // debug temporary print
    snprintf(temp, 130, "(%ld,%ld) cx %ld cy %ld | line %ld | (llen %ld, EOF %i)", (ed.cy - size_header + 1), (ed.cx - left_margin + 1), ed.cx, ed.cy, ed.line, l.llen, l.eof);
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
    if (ev.key <= TB_KEY_F1 && ev.key >= TB_KEY_F12) {
        return;
    }

    if (ev.key == TB_KEY_F1)
        return;

    // TODO handle meta, ctrl, alt ...
    switch (ev.key) {
        case TB_KEY_HOME:
            move_start();
            break;

        case TB_KEY_END:
            move_end();
            break;

        case TB_KEY_ARROW_UP:
            try_move_cursor_up(1);
            break;

        case TB_KEY_PGUP:
            try_move_cursor_up(page_updn_lines);
            break;

        case TB_KEY_ARROW_DOWN:
            try_move_cursor_down(1);
            break;

        case TB_KEY_PGDN:
            try_move_cursor_down(page_updn_lines);
            break;

        case TB_KEY_ARROW_LEFT:
            try_move_cursor_left();
            break;

        case TB_KEY_ARROW_RIGHT:
            try_move_cursor_right();
            break;

        case TB_KEY_ENTER:
            enter_ch();
            break;

        case TB_KEY_TAB:
            tab_ch();
            break;

        case TB_KEY_BACKSPACE:
        case TB_KEY_BACKSPACE2:
            backspace_ch();
            break;

        case TB_KEY_DELETE:
            delete_ch();
            break;

        default:
            insert_ch(ev.ch);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd failed");
        return 1;
    }

    size_t filepathlen = strlen(cwd) + strlen(argv[1]) + 2;
    filepath = malloc(filepathlen);

    if (filepath == NULL) {
        perror("malloc filepath failed.");
        return 1;
    }

    snprintf(filepath, filepathlen, "%s/%s", cwd, argv[1]); // full path

    int ret = tb_init();
    ed = ed_init();

    if (ret) {
        fprintf(stderr, "Could not even TUI, bro.\n");
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
                    if (ev.key == TB_KEY_CTRL_Q || ev.key == TB_KEY_F10)
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
    free(filepath);
    return 0;
}
