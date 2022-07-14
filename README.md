# ttft_bitbang
 A bit-banged TFT library with a tiny footprint

This is a rework of the excellent bit-banged TFT library by "David Johnson-Davies - www.technoblogy.com" where I redefined a few things, tidied it up, and somehow made it more portable. You may use it with any microcontroller with just toggling GPIOs. literally all Arduino, AVR, STM8, STM32, PIC ... micocontrollers. Both files are quite self-explanatory and easy to understand.

**For more info, read the [original post](http://www.technoblogy.com/show?3WAI).**

#### My revisions include:
##### [Rev 0]:
- changed the "int"s to a more restrict variable size of int16_t, int8_t ...
- changed the function names so they won't interfere with other things. 
  every function/variable observable by the main program, starts with "ttft_"
- added a fast "abs" function. you can get rid of that stdlib.h if you don't like it, see the header file.
##### [Rev1, 7-14-2022]:
- the `ttft_init_disp_io()` defers from `ttft_init_disp` now; first one is optional since you can config them in your code and simply call the dispaly routines only.
- a `void ttft_clear_section(int16_t xx, int16_t yy)` is added where a section of screen is deleted quickly using 12bit color data.

## EZ Usage:
1. Copy both the header and the source file to your project folder.
2. Add the source file to your project's hierarchy to be compiled.
3. Read the header file (.h file) carefully.
4. Open source file, there is a *SETUP* area defined; set it according to your MCU and display
5. Fire it up :)

#### Notes:
- if your display has no *cs* pin, let it be. you don't have to do anything about it
- if your display has a *reset* pin, connect it to your MCUs, reset pin
- there are 3 variable available to you to change: text size, fore color(brush color), back color. you can set them in your code, whereever you want.

#### Minimum setup:
```
ttft_init_disp_io (); //Optional, you can init the GPIOs in your code instead
ttft_init_disp();     //Call to initialize display itself.
ttft_clear_disp();
ttft_disp_on();
//...
Waterfall(); // this is a waterfall example from the original library used as a demo
//...
fore_color = ttft_get_color (150,50,100); //RGB color to set as brush color of a shape or text
back_color = ttft_get_color (150,50,100); //RGB color to set as back color of a shape or text
ttft_text_scale = 2; //from now on, text size would be 2
//...
//draw a few shapes
//...
//change the color or text scale if you like
```
![Waterfall example on a ST7735 display](https://github.com/Tiranoid/ttft_bitbang/blob/main/ttft_bitbang.jpg)
