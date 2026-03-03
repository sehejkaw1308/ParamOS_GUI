#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants */
enum vga_color {
    VGA_COLOR_BLACK = 0, VGA_COLOR_BLUE = 1, VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3, VGA_COLOR_RED = 4, VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6, VGA_COLOR_LIGHT_GREY = 7, VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9, VGA_COLOR_LIGHT_GREEN = 10, VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12, VGA_COLOR_LIGHT_MAGENTA = 13, VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

/* GUI Helper: Draw a rectangle/window */
void gui_draw_window(int x, int y, int width, int height, const char* title, uint8_t color, uint8_t title_color) {
    // Draw Window Body
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            terminal_buffer[i * VGA_WIDTH + j] = vga_entry(' ', color);
        }
    }

    // Draw Title Bar
    for (int j = x; j < x + width; j++) {
        terminal_buffer[y * VGA_WIDTH + j] = vga_entry(' ', title_color);
    }

    // Draw Title Text
    int title_len = 0;
    while(title[title_len]) title_len++;
    for (int i = 0; i < title_len && i < width - 2; i++) {
        terminal_buffer[y * VGA_WIDTH + (x + 1 + i)] = vga_entry(title[i], title_color);
    }

    // Draw "X" Close Button
    terminal_buffer[y * VGA_WIDTH + (x + width - 2)] = vga_entry('X', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
}

/* GUI Helper: Draw Taskbar */
void gui_draw_taskbar() {
    uint8_t bar_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY);
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + i] = vga_entry(' ', bar_color);
    }
    // "Start" Button
    const char* start = "[ START ]";
    for(int i = 0; start[i]; i++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + (i + 1)] = vga_entry(start[i], vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_CYAN));
    }
}

void terminal_initialize(void) {
    // Desktop Background (Blue)
    uint8_t desktop_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        terminal_buffer[i] = vga_entry(176, desktop_color); // 176 is a dithering pattern
    }
}

void kernel_main(void) {
    terminal_initialize();

    // Draw a main GUI Window
    gui_draw_window(10, 5, 60, 12, "ParamOS System Settings", 
                    vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY), 
                    vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY));

    // Draw a second smaller "Alert" window
    gui_draw_window(45, 14, 25, 5, "Alert", 
                    vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE), 
                    vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));

    // Draw the Taskbar at the bottom
    gui_draw_taskbar();
    
    // Halt
    while(1);
}
