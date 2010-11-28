#ifndef TEST_H_
#define TEST_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


int hd44780_init();
void hd44780_free();

void hd44780_put_char(u8 c);
void hd44780_put_string(char *c);

void hd44780_goto_xy(int x, int y);
void hd44780_clear();

#endif /* TEST_H_ */
