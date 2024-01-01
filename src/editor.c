#include "nuts.h"

editor_t ed_init(void) {
    return (editor_t){.cx = size_header, .cy = left_margin};
}
