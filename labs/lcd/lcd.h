
#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

#define LCD_PORT PORTD
#define LCD_PINS DDRD
#define LCD_RS_PIN PD2
#define LCD_EN_PIN PD3

void lcd_init (void);
void lcd_cmd(unsigned char cmnd);
void lcd_clear(void);
void lcd_print (char *str);
void lcd_goto_xy(char pos, char row);
void lcd_home(void);

#endif