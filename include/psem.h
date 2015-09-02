/*
--------------------------------------------------------------------------
******************   TESTMETER.EXE SE-240 Meter Comm  *******************
--------------------------------------------------------------------------
	----------------------------------------------------------------------
	Copyright 1997,1998, 1999, 2000 EPRI
		   
	EPRI offers the following for use without fee provided any copy or 
	source derived from this code contains an acknowledgement to EPRI, 
	this notice, and a declaration of its origin.
		   
	EPRI makes no warranty with regard to the software or it's performance 
	and the user accepts the software "AS IS" with all faults.
		
	EPRI DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
	TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
	----------------------------------------------------------------------	


	PSEM.h

	Author				: Dr. Martin J. Burns

	Date of Completion	: December 31, 1999
	Date last modified	: December 31, 1999

	Project				: EPRI SE-240 Meter Communications
	Tabs				: 4 for formatting printouts
	Language			: Ansi C
	Target				: MS-DOS

	Abstract:
	
	This header file contains definitions for the PSEM protocols

	Copyright 1999,2000 EPRI, ALL Rights Reserved


--------------------------------------------------------------------------
**********************	 Revision History.   *****************************
--------------------------------------------------------------------------


	12/31/99		0.0a	Original Version


*/


#ifndef __PSEM_H
#define __PSEM_H


typedef enum {
	PSEM_APP_BASE,
	PSEM_APP_ID,
	PSEM_APP_SESSION
} PSEM_STATE;

typedef enum {
	PSEM_IDENT 			= 0x20,
	PSEM_TERMINATE		= 0x21,
	PSEM_READ 			= 0x30,
	PSEM_READ_OFFSET 	= 0x3f,
	PSEM_WRITE 			= 0x40,
	PSEM_WRITE_OFFSET 	= 0x4f,
	PSEM_LOGON			= 0x50,
	PSEM_SECURITY		= 0x51,
	PSEM_LOGOFF			= 0x52,
	PSEM_NEGOTIATE		= 0x60,
	PSEM_WAIT			= 0x70,
} PSEM_AL;

typedef enum {
	PSEM_EXTERN	= 0,
	PSEM_300	= 1,
	PSEM_600	= 2,
	PSEM_1200	= 3,
	PSEM_2400	= 4,
	PSEM_4800	= 5,
	PSEM_9600	= 6,
	PSEM_14400	= 7,
	PSEM_19200	= 8,
	PSEM_28800	= 9,
	PSEM_56200	= 0x0a,
} PSEM_BAUD;

typedef enum {
	PSEM_RESPONSE_OK 	= 0x00,
	PSEM_RESPONSE_ERR 	= 0x01,
	PSEM_RESPONSE_SNS 	= 0x02,
	PSEM_RESPONSE_ISC 	= 0x03,
	PSEM_RESPONSE_ONP 	= 0x04,
	PSEM_RESPONSE_IAR 	= 0x05,
	PSEM_RESPONSE_BSY 	= 0x06,
	PSEM_RESPONSE_DNR 	= 0x07,
	PSEM_RESPONSE_DLK 	= 0x08,
	PSEM_RESPONSE_RNO 	= 0x09,
	PSEM_RESPONSE_ISSS 	= 0x0a,
	PSEM_RESPONSE_FAIL 	= 0xff,
} PSEM_RESPONSE;

typedef enum {
	PSEM_FRAME_SERVICE 	= 0,
	PSEM_FRAME_RESPONSE	= 1,
} PSEM_FRAME_FMT;

// For DL messaging
#define PSEM_ACK		0x06
#define PSEM_NAK		0x15
#define PSEM_STP		0xee
#define PSEM_CTRL_TGL 	0x20

// For CRC computations
#define INITFCS         0xFFFF  // Initial FCS value
#define GOODFCS         0x470f  // Final good FCS value

// Data link and application timing 
#define INTER_CHARACTER_TIME_OUT	5 
#define CHANNEL_TRAFFIC_TIME_OUT	60
#define WAITFORACK					20
#define WAITFORRESPONSE				20
#define MAX_RETRY					3

// PSEM identify constants
#define PSEM_STD 0
#define PSEM_VER 1
#define PSEM_REV 0
#define PSEM_RSVD 0

typedef struct {
	BYTE stp;			// start flag PSEM_STP
	BYTE fill;			// reserved for MFG, default 0
	BYTE ctrl;			// control byte
	BYTE seq;			// sequence number
	BYTE lengthmsb;		// length of message in bytes
	BYTE lengthlsb;		// length of message in bytes
	BYTE service;	// service token
} typePSEMHeader;		// describes the header of a PSEM frame

#endif
