/* Tiny TFT Graphics Library v3 - see http://www.technoblogy.com/show?3WAI
		
		David Johnson-Davies - www.technoblogy.com - 11th May 2022
		
		CC BY 4.0
		Licensed under a Creative Commons Attribution 4.0 International license: 
		http://creativecommons.org/licenses/by/4.0/
		
		REVISION 1, 6/18/2022
		Tirdad sadri nejad
		--changed the "int"s to a more restrict variable size of int16_t, int8_t ...
		--changed the function names so they won't interfere with other things.
			every function/variable oversable by the main program, starts with "ttft_"
		--added a fast "abs" function. you can get rid of that stdlib.h if you don't like it
*/


#ifndef __TTFT_ST77_BITBANG_H__
#define __TTFT_ST77_BITBANG_H__

#ifdef __cplusplus
extern "C" {
#endif

//Include MCU-specific headers
#include "stm8s.h"
#include "delay.h"

// SETUP THE DISPLAY IN THE SOURCE FILE
// UNCOMMENT THE DISPLAY TYPE IN SOURCE FILE




extern int16_t ttft_fore_color, ttft_back_color;
extern uint8_t ttft_text_scale;

void ttft_init_disp_io (void);
void ttft_clear_disp (void);
void ttft_disp_on (void);
int16_t ttft_get_color (int16_t r, int16_t g, int16_t b);
// Move current plot position to x,y
void ttft_move_cursor (int16_t x, int16_t y);
// Plot point at x,y
void ttft_plot_point (int16_t x, int16_t y);
// Draw a line to x,y
void ttft_draw_line (int16_t x, int16_t y);
void ttft_draw_filled_rect (int16_t w, int16_t h);
void ttft_draw_rect (int16_t w, int16_t h);
void ttft_draw_filled_circle (int16_t radius);
void ttft_draw_circle (int16_t radius);
// Plot an ASCII character with bottom left corner at x,y
void ttft_write_char (char c);
// Plot text starting at the current plot position
void ttft_write( char *p);
void ttft_write_int(int16_t n);
void TestChart (void);
//void BarChart (void);
void Waterfall (void);
#ifdef __cplusplus
}
#endif
#endif