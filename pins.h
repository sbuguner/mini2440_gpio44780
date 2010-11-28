#ifndef PINS_H_
#define PINS_H_


/*
 * Pins configuration:
 *
 * RS -> EINT0 (GPIO pin 15)
 * RW -> EINT1 (GPIO pin 14)
 * E  -> EINT2 (GPIO pin 13)
 * D4 -> EINT3 (GPIO pin 12)
 * D5 -> EINT4 (GPIO pin 11)
 * D6 -> EINT5 (GPIO pin 10)
 * D7 -> EINT6 (GPIO pin 9)
 *
 * GND -> GND (GPIO pin 3)
 * VCC -> +5V (GPIO pin 1)
 * Contrast -> GND (GPIO pin 3)
 *
 */

/* GPIO addresses */
#define PIN_RS 166
#define PIN_RW 165
#define PIN_E  164
#define PIN_D4 163
#define PIN_D5 162
#define PIN_D6 161
#define PIN_D7 160


#endif /* PINS_H_ */
