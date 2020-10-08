
#ifndef LCD_C
#define LCD_C

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

void lcd_init(void)
{
	LCD_DATA_PINS  |= (1 << LCD_DATA_D7) | (1 << LCD_DATA_D6) | (1 << LCD_DATA_D5) | (1 << LCD_DATA_D4);	
	LCD_CTRL_PINS  |= (1 << LCD_CTRL_EN) | (1 << LCD_CTRL_RS);

	LCD_DATA_PINS = 0xFF;
	_delay_ms(15);

	lcd_cmd(0x02);
	lcd_cmd(0x28);
	lcd_cmd(0x0c);
	lcd_cmd(0x06);
	lcd_cmd(0x01);
	_delay_ms(2);
}

void lcd_cmd(unsigned char cmnd)
{
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd & 0xF0);
	LCD_CTRL_PORT &= ~ (1 << LCD_CTRL_RS);
	LCD_CTRL_PORT |= (1 << LCD_CTRL_EN);
	_delay_us(1);
	
	LCD_CTRL_PORT &= ~(1 << LCD_CTRL_EN);
	_delay_us(200);
	
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmnd << 4);
	LCD_CTRL_PORT |= (1<<LCD_CTRL_EN);
	_delay_us(1);
	
	LCD_CTRL_PORT &= ~ (1<<LCD_CTRL_EN);
	_delay_ms(2);
}

void lcd_clear(void)
{
	lcd_cmd (0x01);	
	_delay_ms(2);
	
	lcd_cmd (0x80);
}


void lcd_print(char *str)
{
	int i;
	for(i = 0; str[i] != 0; i++)
	{
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (str[i] & 0xF0);
		LCD_CTRL_PORT |= (1 << LCD_CTRL_RS);
		LCD_CTRL_PORT |= (1 << LCD_CTRL_EN);
		_delay_us(1);
	
		LCD_CTRL_PORT &= ~(1 << LCD_CTRL_EN);
		_delay_us(200);
	
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (str[i] << 4);
		LCD_CTRL_PORT |= (1 << LCD_CTRL_EN);
		_delay_us(1);
	
		LCD_CTRL_PORT &= ~(1 << LCD_CTRL_EN);
		_delay_ms(2);
	}
}

void lcd_goto_xy(char pos, char row)
{
	if (row == 0 && pos<16)
		lcd_cmd((pos & 0x0F) | 0x80);
	
	else if (row == 1 && pos < 16)
		lcd_cmd((pos & 0x0F) | 0xC0);
}

void lcd_home(void)
{
	lcd_goto_xy(0, 0);
}

#endif