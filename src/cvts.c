#include "cvts.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// define variables that are stringified number representations of all fmtval_ts
FMTVAL_ENUM_LIST(GENERATE_FMTSTR_DEF)

bool isprintable(char byte) {
    return byte >= 32 && byte <= 127;
}

void cvts_set_text_format(fmtval_t format) {
    printf("\x1b[%um", (uint32_t)format);
}

void cvts_cursor_up(size_t n) {
    printf("\x1b[%luA", n);
}

void cvts_cursor_down(size_t n) {
    printf("\x1b[%luB", n);
}

void cvts_cursor_right(size_t n) {
    printf("\x1b[%luC", n);
}

void cvts_cursor_left(size_t n) {
    printf("\x1b[%luD", n);
}

void cvts_cursor_next_line(size_t n) {
    printf("\x1b[%luE", n);
}

void cvts_cursor_prev_line(size_t n) {
    printf("\x1b[%luF", n);
}

void cvts_cursor_abs_horizontal(size_t n) {
    printf("\x1b[%luG", n);
}

void cvts_cursor_abs_vertical(size_t n) {
    printf("\x1b[%lud", n);
}

vec2_t cvts_get_cursor_pos() {
    #pragma push_macro("SIZE")
    #define SIZE 24

    vec2_t pos = {};
    char input[SIZE] = "";
    struct termios original;
    struct termios changed;

    // change terminal settings
    tcgetattr(STDIN_FILENO, &original);
    changed = original;
    changed.c_lflag &= ~(ICANON | ECHO);
    changed.c_cc[VMIN] = 1;
    changed.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &changed);

    // query cursor position
    printf("\x1b[6n");

    // read response
    size_t idx = 0;
    for (char ch = 0; (ch = getchar()) != 'R' && ch != EOF && idx + 1 < SIZE;) {
        if (isprintable(ch)) {
            input[idx] = ch;
            idx++;
            input[idx] = '\0';
        }
    }

    // restore previous terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &original);

    // pos = {x=0,y=0} if cursor pos query failed
    if (sscanf(input, "[%lu;%lu", &pos.y, &pos.x) != 2)
        pos = (vec2_t){};

    #pragma pop_macro("SIZE")

    return pos;
}

void cvts_set_cursor_pos(vec2_t pos) {
    printf("\x1b[%lu;%luH", pos.y, pos.x);
}

vec2_t cvts_get_terminal_size() {
    vec2_t prevpos = cvts_get_cursor_pos();                     // save previous cursor position
    cvts_cursor_hide();
    cvts_set_cursor_pos((vec2_t){ ULONG_MAX, ULONG_MAX });      // position cursor as far right and down as possible
    vec2_t size = cvts_get_cursor_pos();                        // get cursor position
    size.x++;
    size.y++;
    cvts_set_cursor_pos(prevpos);                               // restore previous cursor position
    cvts_cursor_show();
    return size;
}

void cvts_cursor_blink_enable() {
    printf("\x1b[?12h");
}

void cvts_cursor_blink_disable() {
    printf("\x1b[?12l");
}

void cvts_cursor_show() {
    printf("\x1b[?25h");
}

void cvts_cursor_hide() {
    printf("\x1b[?25l");
}

void cvts_cursor_shape_default() {
    printf("\x1b[0 q");
}

void cvts_cursor_shape_blink_block() {
    printf("\x1b[1 q");
}

void cvts_cursor_shape_steady_block() {
    printf("\x1b[2 q");
}

void cvts_cursor_shape_blink_underline() {
    printf("\x1b[3 q");
}

void cvts_cursor_shape_steady_underline() {
    printf("\x1b[4 q");
}

void cvts_cursor_shape_blink_bar() {
    printf("\x1b[5 q");
}

void cvts_cursor_shape_steady_bar() {
    printf("\x1b[6 q");
}

void cvts_scroll_up(size_t n) {
    printf("\x1b[%luS", n);
}

void cvts_scroll_down(size_t n) {
    printf("\x1b[%luT", n);
}

void cvts_insert_char(size_t n) {
    printf("\x1b[%lu@", n);
}

void cvts_delete_char(size_t n) {
    printf("\x1b[%luP", n);
}

void cvts_erase_char(size_t n) {
    printf("\x1b[%luX", n);
}

void cvts_insert_line(size_t n) {
    printf("\x1b[%luL", n);
}

void cvts_delete_line(size_t n) {
    printf("\x1b[%luM", n);
}

void cvts_erase_in_display(size_t n) {
    printf("\x1b[%luJ", n);
}

void cvts_erase_in_line(size_t n) {
    printf("\x1b[%luK", n);
}

void cvts_line_drawing_enable() {
    printf("\x1b(0");
}

void cvts_line_drawing_disable() {
    printf("\x1b(B");
}

void cvts_set_scroll_region(size_t top, size_t bottom) {
    printf("\x1b[%lu;%lur", top, bottom);
}

void cvts_set_scroll_region_top(size_t top) {
    printf("\x1b[%lu;r", top);
}

void cvts_set_scroll_region_bottom(size_t bottom) {
    printf("\x1b[;%lur", bottom);
}

void cvts_reset_scroll_region() {
    printf("\x1b[;r");
}

void cvts_screen_buffer_alternate()
{
    printf("\x1b[?1049h");
}

void cvts_screen_buffer_main() {
    printf("\x1b[?1049l");
}
