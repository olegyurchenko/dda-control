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
int lcd_width();
int lcd_height();
void lcd_put_line(int y, const char *txt, SCR_ALIGN align);
void lcd_clear();
void lcd_put_char(int x, int y, int ch);
int lcd_get_char(int x, int y);
void lcd_update();
void lcd_add_scroll_text(int y, int x, int hsize, const char *txt);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_LCD_H_1354775428*/

