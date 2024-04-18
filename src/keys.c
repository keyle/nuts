#include "nuts.h"

void handle_key(struct tb_event ev) {
    if (ev.key <= TB_KEY_F1 && ev.key >= TB_KEY_F12) {
        return;
    }

    if (ev.mod & TB_MOD_CTRL & TB_MOD_ALT) {
    } else if (ev.mod & TB_MOD_CTRL) {
        switch (ev.key) {
            case TB_KEY_CTRL_S:
                func_save();
                break;

            // not sure why these are under mod_Ctrl
            case TB_KEY_BACKSPACE:
            case TB_KEY_BACKSPACE2:
                backspace_ch();
                break;
            default:
                break; // unknown CTRL_combo
        }
    } else if (ev.mod & TB_MOD_ALT) {
    } else {
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

            case TB_KEY_DELETE:
                delete_ch(0);
                break;

            default:
                insert_ch(ev.ch);
        }
    }
}
