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
    return;
    for (i = 0; i < width; i++) {
        tb_set_cell(i, 0, 0, 0, FRAME_BG);
        // tb_set_cell(i, 1, 0, 0, SCREEN_BG);
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
        file_t res = read_file_content(ed.file_path);
        if (res.error) {
            tb_shutdown();
            fprintf(stderr, "Could not load file... %s\n", ed.file_path);
            exit(1);
        }
        ed.contents = res;

        status_write("File loaded.");

        return ed.contents.data;
    }
}

uint16_t determine_color(char ch) {
    if (ch == '\'' || ch == '\"')
        return TB_GREEN | TB_BOLD; // strings
    else if (isalpha(ch))
        return FG; // a-Z
    else if (isdigit(ch))
        return TB_GREEN; // numbers
    return TB_BLUE;      // default
}

void draw_text_buffer(void) {
    const char* file = get_buffer_contents();
    size_t draw_line_count = max_height() - size_header;
    size_t skip_lines = ed.scroll_v_offset;
    uint16_t color;
    size_t i = 0, x = 0, y = 0;
    bool within_line_comment = false;
    bool within_string_literal = false;
    bool in_multiline_comment = false;

    while (file[i] != '\0') {
        char ch = file[i];

        // Handle newline
        if (ch == '\n') {
            if (skip_lines > 0) {
                skip_lines--;
            } else {
                // did return line
                within_line_comment = false;
            }
            x = 0;
            y++;
            if (y > draw_line_count + ed.scroll_v_offset)
                break; // stop rendering
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

        if (!within_line_comment && ch == '/' && i + 1 < strlen(file) && file[i + 1] == '*') {
            in_multiline_comment = true;
        }

        if (ch == '/') {
            if (i + 1 < strlen(file) && file[i + 1] == '/') {
                within_line_comment = true;
            }
        }

        if (!within_line_comment && (ch == '\'' || ch == '\"')) {
            within_string_literal = !within_string_literal;
        }

        if (in_multiline_comment && ch == '*' && i + 1 < strlen(file) && file[i + 1] == '/') {
            in_multiline_comment = false;
        }

        // Set cell color based on content
        if (within_line_comment || in_multiline_comment)
            color = TB_MAGENTA;
        else if (within_string_literal)
            color = TB_GREEN;
        else
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
    snprintf(temp, 130, "scroll %ld | max_height %ld | cx %ld cy %ld | line %ld | (llen %ld, EOF %i) %s",
             ed.scroll_v_offset, max_height(), ed.cx, ed.cy, ed.line, l.llen, l.eof, ed.status);
    tb_printf(0, height - 1, TB_BLACK | TB_BOLD | TB_ITALIC, FRAME_BG, temp);

    // tb_printf(0, height - 1, TB_BLACK | TB_BOLD | TB_ITALIC, FRAME_BG, ed.status);
}

void render(void) {
    draw_background();
    draw_text_buffer();
    draw_frame();
    tb_set_cursor(ed.cx, ed.cy);
    print_status_bar();
    tb_present();
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

    int ret = tb_init();
    ed = ed_init();

    size_t filepathlen = strlen(cwd) + strlen(argv[1]) + 2;
    snprintf(ed.file_path, MIN(filepathlen, FILE_PATH_MAX_LEN), "%s/%s", cwd, argv[1]); // full path
    snprintf(ed.file_param, MIN(strlen(argv[1]) + 1, PARAM_MAX_LEN), "%s", argv[1]);

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
    return 0;
}
