#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/types.h>

#include "pins.h"
#include "hd44780.h"


/* Pin direction */
#define DIR_IN 0
#define DIR_OUT 1

/* Register select */
#define RS_CMD 0
#define RS_DATA 1

/* GPIO sysfs paths */
#define SYSFS_BASE "/sys/class/gpio/"
#define SYSFS_EXPORT SYSFS_BASE "export"
#define SYSFS_UNEXPORT SYSFS_BASE "unexport"

#define LCD_START_LINE_1	0x80
#define LCD_START_LINE_2	0xC0

static int init_pins();
static int initialize_pin(int address, int direction);
static int write_value(int address, int value);
static void clock_in_byte(int reg_select, u8 value);
static void clock_in_nibble(int reg_select, u8 value);

#define MAX_PIN_SIZE 10

struct pin_desc {
  int address;
  char *file_name;
  FILE *file;
  int direction;
};

struct pin_desc pins[MAX_PIN_SIZE];

static int init_pins(){
	memset(pins, 0, MAX_PIN_SIZE * sizeof(struct pin_desc));

	if(!initialize_pin(PIN_RS, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_RW, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_E, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_D4, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_D5, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_D6, DIR_OUT)) return 0;
	if(!initialize_pin(PIN_D7, DIR_OUT)) return 0;

	return 1;
}

static int initialize_pin(int address, int direction){

  FILE *file;
  char file_name[100];
  static struct pin_desc *current_desc = pins;

  if((file = fopen(SYSFS_EXPORT, "w")) == NULL){
    printf("Error openining export " SYSFS_EXPORT "\n");
    perror("");
    return 0;
  }

  fprintf(file, "%d\n", address);
  fclose(file);


  snprintf(file_name, 100, "%s%s%d/direction", SYSFS_BASE, "gpio", address);

  if((file = fopen(file_name, "w")) == NULL){
    printf("Error openining %s\n", file_name);
    perror("");
    return 0;
  }

  fprintf(file, "%s\n", (direction == DIR_IN ? "in" : "out"));
  fclose(file);

  snprintf(file_name, 100, "%s%s%d/value", SYSFS_BASE, "gpio", address);

  if((file = fopen(file_name, "w")) == NULL){
    printf("Error openining %s\n", file_name);
    perror("");
    return 0;
  }

  current_desc->file = file;
  current_desc->address = address;
  current_desc->direction = direction;

  current_desc++;
  return 1;
}

static int write_value(int address, int value){

	int i;

	for(i = 0; i < MAX_PIN_SIZE; i++){
		struct pin_desc *desc = &pins[i];
		if(desc->address == address){
			fprintf(desc->file, (value == 0 ? "0" : "1"));
			fflush(desc->file);
			return 0;
		}
	}
	printf("Error writing value %d to address %d\n", address, value);
	return 1;
}


static void clock_in_byte(int reg_select, u8 value){

	write_value(PIN_E, 0);
	write_value(PIN_RS, reg_select);
	write_value(PIN_D4, (value & (1 << 4)) != 0);
	write_value(PIN_D5, (value & (1 << 5)) != 0);
	write_value(PIN_D6, (value & (1 << 6)) != 0);
	write_value(PIN_D7, (value & (1 << 7)) != 0);
	write_value(PIN_E, 1);
	usleep(1);
	write_value(PIN_E, 0);
	write_value(PIN_D4, (value & (1 << 0)) != 0);
	write_value(PIN_D5, (value & (1 << 1)) != 0);
	write_value(PIN_D6, (value & (1 << 2)) != 0);
	write_value(PIN_D7, (value & (1 << 3)) != 0);
	write_value(PIN_E, 1);
	usleep(1);
	write_value(PIN_E, 0);

}

static void clock_in_nibble(int reg_select, u8 value){

	write_value(PIN_E, 0);
	write_value(PIN_RS, reg_select);
	write_value(PIN_D4, (value & (1 << 0)) != 0);
	write_value(PIN_D5, (value & (1 << 1)) != 0);
	write_value(PIN_D6, (value & (1 << 2)) != 0);
	write_value(PIN_D7, (value & (1 << 3)) != 0);
	write_value(PIN_E, 1);
	usleep(1);
	write_value(PIN_E, 0);

}


int hd44780_init(){

	if(!init_pins())
		return 0;

	write_value(PIN_RW, 0);
	usleep(5000);
	clock_in_nibble(RS_CMD, 3);
	usleep(5000);
	clock_in_nibble(RS_CMD, 3);
	usleep(200);
	clock_in_nibble(RS_CMD, 3);
	usleep(200);
	clock_in_nibble(RS_CMD, 2);
	usleep(5000);

	clock_in_byte(RS_CMD,0x28);
	clock_in_byte(RS_CMD,0x08);
	clock_in_byte(RS_CMD,0x01);
	clock_in_byte(RS_CMD,0x06);
	clock_in_byte(RS_CMD,0x0c);

	return 1;

}

void hd44780_free(){
	FILE * file;
	int i;

	hd44780_clear();

	for(i = 0; i < MAX_PIN_SIZE; i++){

		if(pins[i].address != 0){
			fclose(pins[i].file);

			if((file = fopen(SYSFS_UNEXPORT, "w")) == NULL){
				printf("Error openining export " SYSFS_EXPORT "\n");
				perror("");
				continue;
			}

			fprintf(file, "%d\n", pins[i].address);
			fclose(file);
		}

	}

}

void hd44780_put_char(u8 c){
	clock_in_byte(RS_DATA, c);
}

void hd44780_put_string(char *str){
	u8 *ptr = (u8*)str;
	while(*ptr)
		hd44780_put_char(*ptr++);
}

void hd44780_goto_xy(int x, int y){
	int base = (y == 0 ? LCD_START_LINE_1 : LCD_START_LINE_2);
	clock_in_byte(RS_CMD, base + x);
}

void hd44780_clear(){
	clock_in_byte(RS_CMD, 1);
}


