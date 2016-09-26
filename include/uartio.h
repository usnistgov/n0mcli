#ifndef _UARTIO_H_
#define _UARTIO_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int uartio_close(int port) ;
int uartio_open(char *devicename, int rate, char parity, int databits, int stopbits, int options) ;
int uartio_dataready(int port) ;
char uartio_read(int port, char *dst) ;
char uartio_write(int port, char src) ;
int uartio_write_string(int port, char *src) ;
int uartio_kbhit(void) ;
char uartio_getkey(char *dst) ;
struct termio *uartio_get_termsettings(char *devname) ;
int uartio_set_termsettings(char *devname, struct termio *termio) ;
char *uartio_console_name(void) ;
void uartio_sleep(int msec) ;

#endif
