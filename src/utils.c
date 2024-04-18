#include "nuts.h"

void status_write(const char format[static 1], ...) {
    va_list args;
    va_start(args, format);

    // before: snprintf(ed.status, sizeof ed.status, "%s", stuff);
    // Use vsnprintf instead of snprintf to handle the variadic arguments
    vsnprintf(ed.status, sizeof(ed.status), format, args);

    va_end(args);
}
