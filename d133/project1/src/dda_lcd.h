/*----------------------------------------------------------------------------*/
/**
* @pkg dda_lcd
*/
/**
* DDA functions to operate LCD.
*
* Controll LCD ВС1602Е compatible controller.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 06.12.2012  8:30:28<br>
* @pkgdoc dda_lcd
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_LCD_H_1354775428
#define DDA_LCD_H_1354775428
/*----------------------------------------------------------------------------*/
typedef enum
{
  SCR_ALIGN_LEFT,
  SCR_ALIGN_CENTER,
  SCR_ALIGN_RIGHT
} SCR_ALIGN;
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**Init LCD unit*/
void lcd_init();

/**Low level function*/
/**Put char to the LCD*/
void _lcd_putch(char b);
/**Clear LCD*/
void _lcd_clear();
/**Move cursor home*/
void _lcd_home();
/**Move cursor to position*/
void _lcd_move(char pos);

/**LCD high level functions*/
/**Return screen width*/
int lcd_width();
/**Return screen height*/
int lcd_height();
/**Put line into y string (0,1) LSD whith align alignment. Don't forgat lcd_update() after all modifications.*/
void lcd_put_line(int y, const char *txt, SCR_ALIGN align);
/**Put line into LSD bottom, scrolls up old text. Don't forgat lcd_update() after all modifications.*/
void lcd_add_line(const char *txt, SCR_ALIGN align);
/**Clear screen and scrolled text. Don't forgat lcd_update() after all modifications.*/
void lcd_clear();
/**Put single char to screen in X,Y position. Don't forgat lcd_update() after all modifications.*/
void lcd_put_char(int x, int y, int ch);
/**Get char from X,Y position of screen*/
int lcd_get_char(int x, int y);
/**Update LCD - all modification to device.*/
void lcd_update();
/**Add scrolled text. LCD module support up to 4 scrolled text*/
void lcd_add_scroll_text(int y, int x, int hsize, const char *txt);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_LCD_H_1354775428*/

