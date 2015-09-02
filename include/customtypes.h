#ifndef _CUSTOMTYPES_H_
#define _CUSTOMTYPES_H_

typedef int BOOL;
typedef char CHAR;
typedef unsigned char BYTE;			// always 8 bits
typedef unsigned char UINT8;		// always 8 bits
typedef char * STRING;


typedef unsigned int TIME_T;		// use 32 bit resolution timers
#define TIMETBITS 32				// use 32 bits

#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL ((void *)0)

#endif

#endif
