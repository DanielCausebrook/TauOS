#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint8_t scroll = 1; 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
	if(c != '\n') terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH || c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			if(scroll == 1) {
				for(size_t row = 1; row < VGA_HEIGHT; row++){
					for(size_t col = 0; col < VGA_WIDTH; col++) {
						const size_t index = row * VGA_WIDTH + col;
						terminal_buffer[index - VGA_WIDTH] = terminal_buffer[index];
					}
				}
				terminal_row--;
				uint8_t t_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
				for(size_t col = 0; col < VGA_WIDTH; col++) {
					size_t index = terminal_row * VGA_WIDTH + col;
					terminal_buffer[index] = vga_entry(' ', t_color);
				}
			} else terminal_row = 0;
		}
	}
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
	scroll = 0;
	int ps = 5;
	char ch[] = {'-', '@', '#', '*', 'O'};
	double x[] = {0.0, 0.0, 0.0, 0.0, 0.0};
	double y[] = {0.0, 0.0, 0.0, 0.0, 0.0};
	double vx[] = {0.85, 1.1, 0.7, 1.5, 1.3};
	double vy[] = {0.65, 1.0, 1.25, 0.9, 1.1};
	while(true) {
		
		for(int j = 0; j < (int)  VGA_HEIGHT; j++){
			for(int i = 0; i < (int) VGA_WIDTH; i++) {
				uint8_t hasP = 0;
				for(int p = 0; p < ps; p++) {
					if(hasP == 0 && i == (int) x[p] && j == (int) y[p]) {
						terminal_putchar(ch[p]);
						hasP = 1;
					}
				}
				if(!hasP) terminal_putchar(' ');
			}
		}
		for(int p = 0; p < ps; p++) {
			if(x[p] + vx[p] >= (double) VGA_WIDTH  || -vx[p] > x[p]) vx[p] = -vx[p];
			if(y[p] + vy[p] >= (double) VGA_HEIGHT || -vy[p] > y[p]) vy[p] = -vy[p]; 
			x[p] = (x[p] + vx[p]);
			y[p] = (y[p] + vy[p]);
		}
		for(volatile uint32_t i = 0; i < 3000000; i++);
	}
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\nI can do newlines!\n");
	terminal_writestring("Newline!\n");

}
