#pragma once

/**
 * Console Virtual Terminal Sequences
 * https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */

#include <stdbool.h>
#include <stddef.h>

/**
 * Struct for a two dimensional vector with an x and a y component of type size_t.
 */
typedef struct vec2_t {
    size_t x;
    size_t y;
} vec2_t;

#define FMTVAL_ENUM_LIST(macro) \
    macro(FMTVAL_DEFAULT          ,   0) \
    macro(FMTVAL_BRIGHT           ,   1) \
    macro(FMTVAL_BOLD             ,   1) \
    macro(FMTVAL_NO_BRIGHT        ,  22) \
    macro(FMTVAL_NO_BOLD          ,  22) \
    macro(FMTVAL_UNDERLINE        ,   4) \
    macro(FMTVAL_NO_UNDERLINE     ,  24) \
    macro(FMTVAL_NEGATIVE         ,   7) \
    macro(FMTVAL_POSITIVE         ,  27) \
    macro(FMTVAL_FG_BLACK         ,  30) \
    macro(FMTVAL_FG_RED           ,  31) \
    macro(FMTVAL_FG_GREEN         ,  32) \
    macro(FMTVAL_FG_YELLOW        ,  33) \
    macro(FMTVAL_FG_BLUE          ,  34) \
    macro(FMTVAL_FG_MAGENTA       ,  35) \
    macro(FMTVAL_FG_CYAN          ,  36) \
    macro(FMTVAL_FG_WHITE         ,  37) \
    macro(FMTVAL_FG_DEFAULT       ,  39) \
    macro(FMTVAL_BG_BLACK         ,  40) \
    macro(FMTVAL_BG_RED           ,  41) \
    macro(FMTVAL_BG_GREEN         ,  42) \
    macro(FMTVAL_BG_YELLOW        ,  43) \
    macro(FMTVAL_BG_BLUE          ,  44) \
    macro(FMTVAL_BG_MAGENTA       ,  45) \
    macro(FMTVAL_BG_CYAN          ,  46) \
    macro(FMTVAL_BG_WHITE         ,  47) \
    macro(FMTVAL_BG_DEFAULT       ,  49) \
    macro(FMTVAL_FG_BRIGHT_BLACK  ,  90) \
    macro(FMTVAL_FG_BRIGHT_RED    ,  91) \
    macro(FMTVAL_FG_BRIGHT_GREEN  ,  92) \
    macro(FMTVAL_FG_BRIGHT_YELLOW ,  93) \
    macro(FMTVAL_FG_BRIGHT_BLUE   ,  94) \
    macro(FMTVAL_FG_BRIGHT_MAGENTA,  95) \
    macro(FMTVAL_FG_BRIGHT_CYAN   ,  96) \
    macro(FMTVAL_FG_BRIGHT_WHITE  ,  97) \
    macro(FMTVAL_BG_BRIGHT_BLACK  , 100) \
    macro(FMTVAL_BG_BRIGHT_RED    , 101) \
    macro(FMTVAL_BG_BRIGHT_GREEN  , 102) \
    macro(FMTVAL_BG_BRIGHT_YELLOW , 103) \
    macro(FMTVAL_BG_BRIGHT_BLUE   , 104) \
    macro(FMTVAL_BG_BRIGHT_MAGENTA, 105) \
    macro(FMTVAL_BG_BRIGHT_CYAN   , 106) \
    macro(FMTVAL_BG_BRIGHT_WHITE  , 107)

// generates an enum entry with the given name and value followed by a comma
#define GENERATE_ENUM(name, value) name = value,

/**
 * Enum to identify the values of specific text formats.
 * The extended text color formats are not supported.
 */
typedef enum fmtval_t {
    FMTVAL_ENUM_LIST(GENERATE_ENUM)
} fmtval_t;

// Generates a const char* variable definition of the name FMTSTR_##name with the format string literal for the given value.
#define GENERATE_FMTSTR_DEF(name, value) const char* FMTSTR_##name = "\x1b[" #value "m";

// Generates an extern const char* variable declaration of the name FMTSTR_##name.
#define GENERATE_FMTSTR_DECL(name, value) extern const char* FMTSTR_##name;

// declare extern const char* variables that are stringified number representations of all fmtval_ts
FMTVAL_ENUM_LIST(GENERATE_FMTSTR_DECL)

/**
 * Expands to the FMTSTR_##fmtval variable for the given format value.
 * fmtval must be an enum literal for this macro to work.
 */
#define FMT(fmtval) FMTSTR_##fmtval

/**
 * Checks if the given byte is a printable character (32 ' ' 0x20 <= byte <= 126 '~' 0x7e)
 * @param byte The byte that should be checked.
 * @return true if the given byte is a printable character, false otherwise.
 */
bool isprintable(char byte);

/**
 * Sets the text format to the given fmtval_t.
 * @param format The fmtval_t the text format should be set to.
 */
void cvts_set_text_format(fmtval_t format);

/**
 * Moves the cursor up by n.
 * @param n The number of lines the cursor is moved up.
 */
void cvts_cursor_up(size_t n);

/**
 * Moves the cursor down by n.
 * @param n The number of lines the cursor is moved down.
 */
void cvts_cursor_down(size_t n);

/**
 * Moves the cursor right by n.
 * @param n The number of columns the cursor is moved right.
 */
void cvts_cursor_right(size_t n);

/**
 * Moves the cursor left by n.
 * @param n The number of columns the cursor is moved left.
 */
void cvts_cursor_left(size_t n);

/**
 * Moves the cursor down n lines from the current position.
 * @param n The number of lines the cursor is moved down.
 */
void cvts_cursor_next_line(size_t n);

/**
 * Moves the cursor up n lines from the current position.
 * @param n The number of lines the cursor is moved down.
 */
void cvts_cursor_prev_line(size_t n);

/**
 * Moves the cursor to the nth position horizontally in the current line.
 * @param n The column index of the target position.
 */
void cvts_cursor_abs_horizontal(size_t n);

/**
 * Moves the cursor to the nth position vertically in the current column.
 * @param n The row index of the target position.
 */
void cvts_cursor_abs_vertical(size_t n);

/**
 * Retrieves the current position of the cursor.
 * @return The column (x) and row (y) the cursor is positioned in, {x=0,y=0} if the cursor position query failed.
 */
vec2_t cvts_get_cursor_pos();

/**
 * Sets the cursor position to the given coordinate.
 * @param pos The coordinate the cursor should be positioned at (x = column, y = row).
 */
void cvts_set_cursor_pos(vec2_t pos);

/**
 * Retrieves the size of the terminal
 * @return The number of columns (x) and rows (y) of the terminal.
 */
vec2_t cvts_get_terminal_size();

/**
 * Enables cursor blinking.
 */
void cvts_cursor_blink_enable();

/**
 * Disables cursor blinking.
 */
void cvts_cursor_blink_disable();

/**
 * Shows the cursor.
 */
void cvts_cursor_show();

/**
 * Hides the cursor.
 */
void cvts_cursor_hide();

/**
 * Changes the cursor shape to the default.
 */
void cvts_cursor_shape_default();

/**
 * Changes the cursor shape to blinking block.
 */
void cvts_cursor_shape_blink_block();

/**
 * Changes the cursor shape to steady block.
 */
void cvts_cursor_shape_steady_block();

/**
 * Changes the cursor shape to blinking underline.
 */
void cvts_cursor_shape_blink_underline();

/**
 * Changes the cursor shape to steady underline.
 */
void cvts_cursor_shape_steady_underline();

/**
 * Changes the cursor shape to blinking bar.
 */
void cvts_cursor_shape_blink_bar();

/**
 * Changes the cursor shape to steady bar.
 */
void cvts_cursor_shape_steady_bar();

/**
 * Scrolls up n lines. Also known as pan down, new lines fill in from the bottom of the screen.
 * @param n The number of lines to scroll up.
 */
void cvts_scroll_up(size_t n);

/**
 * Scrolls down n lines. Also known as pan up, new lines fill in from the top of the screen.
 * @param n The number of lines to scroll down.
 */
void cvts_scroll_down(size_t n);

/**
 * Inserts n spaces at the current cursor position, shifting all existing text to the right.
 * Text exiting the screen to the right is removed.
 * @param n The number of inserted spaces.
 */
void cvts_insert_char(size_t n);

/**
 * Deletes n characters at the current cursor position, shifting in space characters from the right edge of the screen.
 * @param n The number of deleted characters.
 */
void cvts_delete_char(size_t n);

/**
 * Erases n characters from the current cursor position by overwriting them with a space character.
 * @param n The number of erased characters.
 */
void cvts_erase_char(size_t n);

/**
 * Inserts n lines into the buffer at the cursor position.
 * The line the cursor is on, and lines below it, will be shifted downwards.
 * @param n The number of inserted lines.
 */
void cvts_insert_line(size_t n);

/**
 * Deletes n lines from the buffer, starting with the row the cursor is on.
 * @param n The number of deleted lines.
 */
void cvts_delete_line(size_t n);

/**
 * Replace all text in the current viewport/screen specified by <n> with space characters.
 * Valid values for n are:
 * 0 - erases from the current cursor position (inclusive) to the end of the display.
 * 1 - erases from the beginning of the display up to and including the current cursor position.
 * 2 - erases the entire display.
 */
void cvts_erase_in_display(size_t n);

/**
 * Replace all text in the current viewport/screen specified by <n> with space characters.
 * Valid values for n are:
 * 0 - erases from the current cursor position (inclusive) to the end of the line.
 * 1 - erases from the beginning of the line up to and including the current cursor position.
 * 2 - erases the entire line.
 */
void cvts_erase_in_line(size_t n);

/**
 * Enables DEC Line Drawing Mode
 */
void cvts_line_drawing_enable();

/**
 * Disables DEC Line Drawing Mode. (Enables US ASCII Mode (Default))
 */
void cvts_line_drawing_disable();

/**
 * Sets the scrolling region
 * @param top The first row in the scroll region (inclusive)
 * @param bottom The last row in the scroll region (inclusive)
 */
void cvts_set_scroll_region(size_t top, size_t bottom);

/**
 * Sets the top row of the scrolling region
 * @param top The first row in the scroll region (inclusive)
 */
void cvts_set_scroll_region_top(size_t top);

/**
 * Sets the bottom row of the scrolling region.
 * @param bottom The last row in the scroll region (inclusive)
 */
void cvts_set_scroll_region_bottom(size_t bottom);

/**
 * Resets the scrolling region to default (top = 1, bottom = current viewport height).
 */
void cvts_reset_scroll_region();

/**
 * Switches to a new alternate screen buffer.
 */
void cvts_screen_buffer_alternate();

/**
 * Switches to the main screen buffer.
 */
void cvts_screen_buffer_main();
