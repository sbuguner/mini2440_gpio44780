#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "hd44780.h"

void signal_handler(int signal_no){
	hd44780_free();
	exit(0);
}

void setup_signal_handler(){
	struct sigaction setmask;

    sigemptyset( &setmask.sa_mask );
    setmask.sa_handler = signal_handler;
    setmask.sa_flags   = 0;

    sigaction( SIGTERM,  &setmask, (struct sigaction *) NULL );
    sigaction( SIGINT,  &setmask, (struct sigaction *) NULL );
}

int main(int args, char **argv){

	hd44780_init();

	setup_signal_handler();

	hd44780_goto_xy(2, 0);
	hd44780_put_string("Hello World!");
	hd44780_goto_xy(4,1);
	hd44780_put_string("|(^o^)|");

	for(;;);

	return 0;

}
