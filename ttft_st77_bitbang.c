#include "ttft_st77_bitbang.h"

//************************************************************
// SETUP: change these lines according to your preferred setup
//************************************************************

//If you want to use built-in "abs" function, then comment this line:
#define USE_BUILTIN_ABS_FUNC
#ifndef USE_BUILTIN_ABS_FUNC
#include <stdlib.h>
#endif


/*
	PIN SETUP:
	The pin number of the MCU where each LCD pin is connected.
	-- all pins must be members of one port e.g. GPIOC
	-- RESET pin of the LCD should be connected to MCU's reset, or an RC circuit.
*/
#define dc		4		//Some displays call it A0
#define mosi	5		//Some displays call it SDA
#define sck		6		//Some displays call it SCL
#define cs		7		//Some displays hard-wire this pin to GND by themselves. you can leave it here

/*
	MCU SPECIFIC LIBRARY SETUP:
	The pin number of the MCU where each LCD pin is connected.
	-- all pins must be members of one port e.g. GPIOC
*/

//Write the function that delays for milliseconds
#define DELAY_MS(x)			delay_ms(x)					

//Write the functions (registers) that toggle, set Hi, set Lo and set output
//for AVR for example:
/*
#define PORT_TOGGLE(x)  PINB = (x)
#define PORT_LOW(x)     PORTB = PORTB & ~((x));
#define PORT_HIGH(x)    PORTB = PORTB | ((x))
#define PORT_OUTPUT(x)  DDRB = (x)
*/

//For my STM8 mcu:
#define PORT_TOGGLE(x)  GPIOC->ODR ^= (x)		
#define PORT_LOW(x)     GPIOC->ODR &=~(x)
#define PORT_HIGH(x)    GPIOC->ODR |= (x)
#define PORT_OUTPUT(x)  GPIOC->DDR |= (x); GPIOC->CR1 |= (x); GPIOC->CR2 |= (x)

/*
	DISPLAY MODEL SELECTION:
	Uncomment the display you are using
	-- you can add your own displays too.
*/

//int16_t const xsize = 128, ysize = 128, xoff = 2, yoff = 1, invert = 0, rotate = 3, bgr = 1;

// AliExpress 1.44" 128x128 display
// int16_t const xsize = 128, ysize = 128, xoff = 2, yoff = 1, invert = 0, rotate = 3, bgr = 1;

// Adafruit 0.96" 160x80 display
// int16_t const xsize = 160, ysize = 80, xoff = 0, yoff = 24, invert = 0, rotate = 6, bgr = 0;

// AliExpress 0.96" 160x80 display
// int16_t const xsize = 160, ysize = 80, xoff = 1, yoff = 26, invert = 1, rotate = 0, bgr = 1;

// Adafruit 1.8" 160x128 display
// int16_t const xsize = 160, ysize = 128, xoff = 0, yoff = 0, invert = 0, rotate = 0, bgr = 1;

// AliExpress 1.8" 160x128 display (red PCB)
// int16_t const xsize = 160, ysize = 128, xoff = 0, yoff = 0, invert = 0, rotate = 0, bgr = 1;

// AliExpress 1.8" 160x128 display (blue PCB)
 int16_t const xsize = 160, ysize = 128, xoff = 0, yoff = 0, invert = 0, rotate = 6, bgr = 0;

// Adafruit 1.14" 240x135 display
// int16_t const xsize = 240, ysize = 135, xoff = 40, yoff = 53, invert = 1, rotate = 6, bgr = 0;

// AliExpress 1.14" 240x135 display
// int16_t const xsize = 240, ysize = 135, xoff = 40, yoff = 52, invert = 1, rotate = 0, bgr = 0;

// Adafruit 1.3" 240x240 display
// int16_t const xsize = 240, ysize = 240, xoff = 0, yoff = 80, invert = 1, rotate = 5, bgr = 0;

// Adafruit 1.54" 240x240 display
// int16_t const xsize = 240, ysize = 240, xoff = 0, yoff = 80, invert = 1, rotate = 5, bgr = 0;

// AliExpress 1.54" 240x240 display
// int16_t const xsize = 240, ysize = 240, xoff = 0, yoff = 80, invert = 1, rotate = 5, bgr = 0;

// Adafruit 1.9" 320x170 display
// int16_t const xsize = 320, ysize = 170, xoff = 0, yoff = 35, invert = 1, rotate = 0, bgr = 0;

// Adafruit 2.0" 320x240 display
// int16_t const xsize = 320, ysize = 240, xoff = 0, yoff = 0, invert = 1, rotate = 6, bgr = 0;

// Adafruit 2.2" 320x240 display
// int16_t const xsize = 320, ysize = 240, xoff = 0, yoff = 0, invert = 0, rotate = 4, bgr = 1;

// AliExpress 2.4" 320x240 display
// int16_t const xsize = 320, ysize = 240, xoff = 0, yoff = 0, invert = 0, rotate = 2, bgr = 1;

//************************************************************
// END OF SETUP
//************************************************************





//************************************************************
//************************************************************
// Character set for text - stored in program memory
const uint8_t CharMap[96][6]  = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, 
{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, 
{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, 
{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, 
{ 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 }, 
{ 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 }, 
{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, 
{ 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 }, 
{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 }, 
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 }, 
{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, 
{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, 
{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 }, 
{ 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 }, 
{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, 
{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, 
{ 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 }, 
{ 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 }, 
{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 }, 
{ 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 }, 
{ 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 }, 
{ 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, 
{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 }, 
{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, 
{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 }, 
{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, 
{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, 
{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 }, 
{ 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 }, 
{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, 
{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, 
{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 }, 
{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, 
{ 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 }, 
{ 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 }, 
{ 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00 }, 
{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, 
{ 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00 }, 
{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, 
{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x00, 0x03, 0x07, 0x08, 0x00, 0x00 }, 
{ 0x20, 0x54, 0x54, 0x78, 0x40, 0x00 }, 
{ 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x28, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00 }, 
{ 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 }, 
{ 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00 }, 
{ 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00 }, 
{ 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00 }, 
{ 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 }, 
{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 }, 
{ 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00 }, 
{ 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00 }, 
{ 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 }, 
{ 0x48, 0x54, 0x54, 0x54, 0x24, 0x00 }, 
{ 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00 }, 
{ 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 }, 
{ 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 }, 
{ 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 }, 
{ 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 }, 
{ 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00 }, 
{ 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 }, 
{ 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x77, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 }, 
{ 0x00, 0x06, 0x09, 0x06, 0x00, 0x00 },  // degree symbol = '~'
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }
};

// EXTENDED FUNCTIONS
uint16_t tft_abs_16(int16_t inValue)
{
	uint16_t temp = inValue >> 15;     // make a mask of the sign bit
  inValue ^= temp;                   // toggle the bits if value is negative
  inValue += temp & 1;
	return inValue;
}
// TFT colour display **********************************************

#define CASET			0x2A // Define column address
#define RASET			0x2B // Define row address
#define RAMWR			0x2C // Write to display RAM

#define White 		0xFFFF
#define Black			0

// Current plot position and colours
int16_t xpos, ypos;
int16_t ttft_fore_color = White;
int16_t ttft_back_color = Black;
uint8_t ttft_text_scale = 1;     // Text ttft_text_scale

// Send a byte to the display

void Data (uint8_t d) {
	uint8_t bit;
  for (bit = 0x80; bit; bit >>= 1) {
    PORT_TOGGLE(1<<sck);
    if (d & bit) PORT_HIGH(1<<mosi); else PORT_LOW(1<<mosi);
    PORT_TOGGLE(1<<sck);
  }
}

// Send a command to the display
void Command (uint8_t c) {
  PORT_TOGGLE(1<<dc);
  Data(c);
  PORT_TOGGLE(1<<dc);
}

// Send a command followed by two data words
void Command2 (uint8_t c, uint16_t d1, uint16_t d2) {
  PORT_TOGGLE(1<<dc);
  Data(c);
  PORT_TOGGLE(1<<dc);
  Data(d1>>8); Data(d1); Data(d2>>8); Data(d2);
}
  
void ttft_init_disp_io (void) {
  PORT_OUTPUT(1<<dc | 1<<cs | 1<<mosi | 1<<sck); // All outputs
  PORT_HIGH(1<<dc | 1<<cs | 1<<sck);       // Outputs high
  PORT_TOGGLE(1<<cs);
  Command(0x01);                           // Software reset
  DELAY_MS(250);                              // delay 250 ms
  Command(0x36); Data(rotate<<5 | bgr<<3); // Set orientation and rgb/bgr
  Command(0x3A); Data(0x55);               // Set color mode - 16-bit color
  Command(0x20+invert);                    // Invert
  Command(0x11);                           // Out of sleep mode
  DELAY_MS(150);
  PORT_TOGGLE(1<<cs);
}

void ttft_disp_on (void) {
  PORT_TOGGLE(1<<cs);
  Command(0x29);                           // Display on
  DELAY_MS(150);
  PORT_TOGGLE(1<<cs);
}

void ttft_clear_disp (void) {
	uint16_t i,j;
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff, yoff + ysize - 1);
  Command2(RASET, xoff, xoff + xsize - 1);
  Command(0x3A); Data(0x03);               // 12-bit colour
  Command(RAMWR);                          // Leaves mosi low
  for (i=0; i<xsize*4; i++) {
    for (j=0; j<ysize*3; j++) {
    PORT_TOGGLE(1<<sck);
    PORT_TOGGLE(1<<sck);
    }
  }
  Command(0x3A); Data(0x05);               // ttft_back_color to 16-bit colour
  PORT_TOGGLE(1<<cs);
}

int16_t ttft_get_color (int16_t r, int16_t g, int16_t b) {
  return (r & 0xf8)<<8 | (g & 0xfc)<<3 | b>>3;
}

// Move current plot position to x,y
void ttft_move_cursor (int16_t x, int16_t y) {
  xpos = x; ypos = y;
}

// Plot point at x,y
void ttft_plot_point (int16_t x, int16_t y) {
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff+y, yoff+y);
  Command2(RASET, xoff+x, xoff+x);
  Command(RAMWR); Data(ttft_fore_color>>8); Data(ttft_fore_color & 0xff);
  PORT_TOGGLE(1<<cs);
}

// Draw a line to x,y
void ttft_draw_line (int16_t x, int16_t y) {
  int16_t sx, sy, e2, err;
	#ifdef USE_BUILTIN_ABS_FUNC
  int16_t dx = tft_abs_16(x - xpos);
  int16_t dy = tft_abs_16(y - ypos);
	#else
	int16_t dx = abs(x - xpos);
  int16_t dy = abs(y - ypos);
	#endif
  if (xpos < x) sx = 1; else sx = -1;
  if (ypos < y) sy = 1; else sy = -1;
  err = dx - dy;
  for (;;) {
    ttft_plot_point(xpos, ypos);
    if (xpos==x && ypos==y) return;
    e2 = err<<1;
    if (e2 > -dy) { err = err - dy; xpos = xpos + sx; }
    if (e2 < dx) { err = err + dx; ypos = ypos + sy; }
  }
}

void ttft_draw_filled_rect (int16_t w, int16_t h) {
	uint16_t i,j;
	uint8_t hi,lo;
  PORT_TOGGLE(1<<cs);
  Command2(CASET, ypos+yoff, ypos+yoff+h-1);
  Command2(RASET, xpos+xoff, xpos+xoff+w-1);
  Command(RAMWR);
  hi = ttft_fore_color>>8;
  lo = ttft_fore_color & 0xff;
  for (i=0; i<w; i++) {
    for (j=0; j<h; j++) {
      Data(hi); Data(lo);
    }
  }
  PORT_TOGGLE(1<<cs);
}

void ttft_draw_rect (int16_t w, int16_t h) {
  int16_t x1 = xpos, y1 = ypos;
  ttft_draw_filled_rect(w-1, 1); ttft_move_cursor(x1, y1+1);
  ttft_draw_filled_rect(1, h-1); ttft_move_cursor(x1+1, y1+h-1);
  ttft_draw_filled_rect(w-1, 1); ttft_move_cursor(x1+w-1, y1);
  ttft_draw_filled_rect(1, h-1);
  xpos = x1; ypos = y1;
}

void ttft_draw_filled_circle (int16_t radius) {
  int16_t x1 = xpos, y1 = ypos, dx = 1, dy = 1;
  int16_t x = radius - 1, y = 0;
  int16_t err = dx - (radius<<1);
  while (x >= y) {
    ttft_move_cursor(x1-x, y1+y); ttft_draw_filled_rect(x<<1, 1);
    ttft_move_cursor(x1-y, y1+x); ttft_draw_filled_rect(y<<1, 1);
    ttft_move_cursor(x1-y, y1-x); ttft_draw_filled_rect(y<<1, 1);
    ttft_move_cursor(x1-x, y1-y); ttft_draw_filled_rect(x<<1, 1);
    if (err > 0) {
      x = x - 1; dx = dx + 2;
      err = err - (radius<<1) + dx;
    } else {
      y = y + 1; err = err + dy;
      dy = dy + 2;
    }
  }
  xpos = x1; ypos = y1;
}

void ttft_draw_circle (int16_t radius) {
  int16_t x1 = xpos, y1 = ypos, dx = 1, dy = 1;
  int16_t x = radius - 1, y = 0;
  int16_t err = dx - (radius<<1);
  while (x >= y) {
    ttft_plot_point(x1-x, y1+y); ttft_plot_point(x1+x, y1+y);
    ttft_plot_point(x1-y, y1+x); ttft_plot_point(x1+y, y1+x);
    ttft_plot_point(x1-y, y1-x); ttft_plot_point(x1+y, y1-x);
    ttft_plot_point(x1-x, y1-y); ttft_plot_point(x1+x, y1-y);
    if (err > 0) {
      x = x - 1; dx = dx + 2;
      err = err - (radius<<1) + dx;
    } else {
      y = y + 1; err = err + dy;
      dy = dy + 2;
    }
  }
}

// Plot an ASCII character with bottom left corner at x,y
void ttft_write_char (char c) {
  int16_t colour,xx,xr,yy,yr;
  PORT_TOGGLE(1<<cs);
  Command2(CASET, yoff+ypos, yoff+ypos+8*ttft_text_scale-1);
  Command2(RASET, xoff+xpos, xoff+xpos+6*ttft_text_scale-1);
  Command(RAMWR);
  for (xx=0; xx<6; xx++) {
    int16_t bits = CharMap[c-32][xx];
    for (xr=0; xr<ttft_text_scale; xr++) {
      for (yy=0; yy<8; yy++) {
        if (bits>>(7-yy) & 1) colour = ttft_fore_color; else colour = ttft_back_color;
        for (yr=0; yr<ttft_text_scale; yr++) {
          Data(colour>>8); Data(colour & 0xFF);
        }
      }
    }
  }
  PORT_TOGGLE(1<<cs);
  xpos = xpos + 6*ttft_text_scale;
}

// Plot text starting at the current plot position
void ttft_write( char *p) {
  while (1) 
	{
    char c = *(p++);
    if (c == 0) return;
    ttft_write_char(c);
  }
}

void ttft_write_int(int16_t n) {
  bool lead = false;
	int d = 10000;
  for (d; d>0; d = d/10) {
    char j = (n/d) % 10;
    if (j!=0 || lead || d==1) { ttft_write_char(j + '0'); lead = true; }
  }
}

void TestChart (void) {
  ttft_draw_rect(xsize, ysize);
  ttft_text_scale = 2;
  ttft_fore_color = ttft_get_color(255, 0, 0);
  ttft_move_cursor((xsize-40)/2, (ysize-64)/2); ttft_write("Khers");
  ttft_text_scale = 1;
}

// Demos **********************************************
/*
void BarChart (void) {
  int x0 = 0, y0 = 0, w = xsize, h = ysize, x1 = 15, y1 = 11;
  ttft_move_cursor(x0+(w-x1-90)/2+x1, y0+h-8); ttft_write(PSTR("Sensor Readings"));
  // Horizontal axis
  int xinc = (w-x1)/20;
  ttft_move_cursor(x0+x1, y0+y1); ttft_draw_line(x0+w-1, y0+y1);
  for (int i=0; i<=20; i=i+4) {
    int mark = x1+i*xinc;
    ttft_move_cursor(x0+mark, y0+y1); ttft_draw_line(x0+mark, y0+y1-2);
    // Draw histogram
    if (i != 20) {
      int bar = xinc*4/3;
      for (int b=2; b>=0; b--) {
        ttft_fore_color = ttft_get_color(255, 127*b, 0); // Red, Orange, Yellow 
        ttft_move_cursor(x0+mark+bar*b-b+1, y0+y1+1); FillRect(bar, 5+random(h-y1-20));
      }
      ttft_fore_color = White;
    }
    if (i > 9) ttft_move_cursor(x0+mark-7, y0+y1-11); else ttft_move_cursor(x0+mark-3, y0+y1-11);
    ttft_write_int(i);
  }
  // Vertical axis
  int yinc = (h-y1)/20;
  ttft_move_cursor(x0+x1, y0+y1); ttft_draw_line(x0+x1, y0+h-1);
  for (int i=0; i<=20; i=i+5) {
    int mark = y1+i*yinc;
    ttft_move_cursor(x0+x1, y0+mark); ttft_draw_line(x0+x1-2, y0+mark);
    if (i > 9) ttft_move_cursor(x0+x1-15, y0+mark-4); else ttft_move_cursor(x0+x1-9, y0+mark-4);
    ttft_write_int(i);
  }
}
*/

void Waterfall (void) {
  int x0 = 0, y0 = 0, w = xsize, h = ysize, x1 = 15, y1 = 11;
  int factor = 5160/h*10;
	int xinc, yinc, i,y;
  ttft_move_cursor(x0+(w-x1-60)/2+x1, y0+h-8);
  // Horizontal axis
  xinc = (w-x1-15)/30;
  ttft_move_cursor(x0+x1, y0+y1); ttft_draw_line(x0+x1+xinc*20, y0+y1);
  for (i=0; i<=20; i=i+5) {
    int mark = x1+i*xinc;
    ttft_move_cursor(x0+mark, y0+y1); ttft_draw_line(x0+mark, y0+y1-2);
    if (i > 9) ttft_move_cursor(x0+mark-7, y0+y1-11); else ttft_move_cursor(x0+mark-3, y0+y1-11);
    ttft_write_int(i);
  }
  // Vertical axis
  yinc = (h-y1)/20;
  ttft_move_cursor(x0+x1, y0+y1); ttft_draw_line(x0+x1, y0+h-1);
  for (i=0; i<=20; i=i+5) {
    int mark = y1+i*yinc;
    ttft_move_cursor(x0+x1, y0+mark); ttft_draw_line(x0+x1-2, y0+mark);
    if (i > 9) ttft_move_cursor(x0+x1-15, y0+mark-4); else ttft_move_cursor(x0+x1-9, y0+mark-4);
    ttft_write_int(i);
  }
  // Diagonal axis
  yinc = xinc/2;
  // ttft_move_cursor(x0+x1, y0+y1); ttft_draw_line(x0+x1+10*xinc, y0+y1+10*xinc);
  ttft_move_cursor(x0+x1+20*xinc, y0+y1); ttft_draw_line(x0+x1+30*xinc, y0+y1+10*xinc);
  for (i=0; i<=20; i=i+5) {
    ttft_move_cursor(x0+x1+20*xinc+i*xinc/2, y0+y1+i*xinc/2);
    ttft_draw_line(x0+x1+20*xinc+i*xinc/2+3, y0+y1+i*xinc/2);
    ttft_move_cursor(x0+x1+20*xinc+i*xinc/2+6, y0+y1+i*xinc/2-4); ttft_write_int(i);
  }
  // Plot data
  for (y=20; y>=0; y--) {
    for (i=0; i<=20; i++) {
      int fn0 = 180-(i-10)*(i-10)-(y-10)*(y-10);
      int fn1 = 180-(i+1-10)*(i+1-10)-(y-10)*(y-10);
      ttft_fore_color = ttft_get_color(255, 255, 0);
      ttft_move_cursor(x0+x1+y*yinc+i*xinc, y0+y1+y*yinc+fn0*fn0/factor);
      ttft_draw_line(x0+x1+y*yinc+(i+1)*xinc, y0+y1+y*yinc+fn1*fn1/factor);
      ttft_fore_color = White;
    }
  }
}