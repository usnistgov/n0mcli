/*
--------------------------------------------------------------------------
******************   SE240CLI.EXE SE-240 Meter Comm  *******************
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

	SE240CLI.c

	Author				: Dr. Martin J. Burns

	Date of Completion	: December 31, 1999
	Date last modified	: December 31, 1999

	Project				: EPRI SE-240 Meter Communications
	Tabs				: 4 for formatting printouts
	Language			: Ansi C, Microsoft VCC 1.52
	Target				: MS-DOS

	Abstract:
	
	This project is a test application that implements the 
	ANSI C12.18 protocol for use with communications between the SE240 
	meter and its "plug-in" units.
	
	This version is a generic client for communications with an SE240 meter.
	The philosophy of this program is to be a minimal and bare bones implementation.  It is
	designed to compile down to the smallest possible footprint in ROM and RAM on an embedded
	platform.
	

	===========================================================================
	=====	Compilation options
	===========================================================================


	===========================================================================
	=====	Physical and DataLink layer characteristics
	===========================================================================
	The following apply:
	
		DATA TYPE:					Asynchronous, serial full duplex
		DATA FORMAT:				8 data bits, 1 start bit, 1 stop bit, no parity
		DATA POLARITY:				<0.8V on, start bit, space, logical 0
									>2.5V off, stop bit, mark, logical 1, quiescent state
		DATA RATE:					9600 baud.
		NUMBER OF PACKETS:			One (1) packet outstanding for transmit or receive.
		PACKET SIZE:				Default maximum packet size is 64 bytes
		CHANNEL TRAFFIC TIME-OUT:	6 seconds
		INTER-CHARACTER TIME-OUT:	500 milliseconds
		RESPONSE TIME-OUT:			2 seconds
		TURN-AROUND TIME:			175 microseconds
		
		
	===========================================================================
	=====	C12.18 Data Link Layer Syntax:
	===========================================================================
		
	The following Backus Nauer Form, BNF, describes the syntax of packets:
	<PACKET>		::= <STP> <RESERVED> <CTRL> <SEQ_NBR> <LENGTH> <DATA> <CRC>
	<PULSETABLE>	::= [<LENGTH>] <DATA> <CRC>
	<STP>			::= EEH		Start of packet character
	<RESERVED>		::= <uint8_t>	Reserved for manufacturer or utility use.(Default 0x00)
	<CTRL>			::= <uint8_t>	Control field:
							Bit 7. If true packet is part of segmented transmission.
							Bit 6. If true then first of a multi-packet transmission.
							Bit 5. Represents a toggle bit to reject duplicate packets.
								This bit is toggled for each new packet sent.
								Retransmitted packets keep the same state
							Bits 0 to 4, Reserved

	<SEQ_NBR>		::= <uint8_t>	Number that is decremented by one for each new packet sent.
	
	<LENGTH>		::= <WORD16>Number of bytes of data in packet

	<DATA>			::= <uint8_t>+	<length> number of bytes of actual packet data.(MAX 8138)

	<CRC>			::= <WORD16> CCITT CRC standard polynomial X16 + X12 + X5 + 1
	
	<PULSETRIG>		::= <BREAK>  Break of 4 character time or longer, triggers transmission
								of Table 84
	<ACK>			::= 06H      Positive acknowledgement of frame
	
	<NAK>			::= 15H	  	 Negative acknowledgement of frame for bad framing or checksum
								


	===========================================================================
	=====	C12.18 PSEM Application Layer Syntax:
	===========================================================================
		

	<requests>	::=	<ident> |	{ Identification request }
					<read> |	{ Read request }
					<write> |	{ Write request }
					<logon> |	{ Logon request }
					<security> |	{ Security request }
					<logoff> |	{ Logoff request }
					<negotiate> |	{ Negotiate request }
					<wait> |	{ Wait request }
					<terminate>	{ Terminate request }
	
	<responses>	::=	<ident_r> |	{ Identification response }
					<read_r > |	{ Read response }
					<write_r > |	{ Write response }
					<logon_r > |	{ Logon response }
					<security_r> |	{ Security response }
					<logoff_r > |	{ Logoff response }
					<negotiate_r > |	{ Negotiate response }
					<wait_r > |	{ Wait response }
					<terminate_r >	{ Terminate response }
	
	PSEM requests always include a one byte request code.  Code numbers are assigned as follows:
		00H-1FH	Codes shall not be used to avoid confusion with response
			codes
		
		20H-7FH	Codes are available for use within optical port protocol
		
		80H-FFH	Codes shall be reserved for protocol extensions
		
	PSEM responses always include a one byte response code. These codes are defined as follows:
	
	<nok>	::=	<err>|<sns>|<isc>|<onp>|<iar>|<bsy>|<dnr>|<dlk>|<rno>|<isss>
	
	<ok>	::=	00H	Acknowledge - No problems, request accepted.}
	
	<err>	::=	01H	Error - rejection of the received service reques
	
	<sns>	::=	02H	Service Not Supported
	
	<isc>	::=	03H	Insufficient Security Clearance
	
	<onp>	::=	04H	Operation Not Possible

	<iar>	::=	05H	Inappropriate Action Requested
	
	<bsy>	::=	06H	Device Busy
	
	<dnr>	::=	07H	Data Not Ready
	
	<dlk>	::=	08H	Data Locked
	
	<rno>	::= 09H	Renegotiate request
	
	<isss>	::= 0AH	Invalid Service Sequence State

	0BH-1FH	{Codes are currently undefined, 
				but are available for use within optical port protocol}

	20H-7FH	{Codes shall not be used to avoid confusion with request codes}
    
	80H-FFH	{Codes shall be reserved for protocol extensions}
		
		
	===========================================================================
	=====	Application Layer Messages:
	===========================================================================
	
	===================================
	=====	ident
	===================================
	Identification request
		<ident>			::=	20H
		
	Identification Response:
		<ident_r>		::=	<err> |	<bsy> |	<isss> | <ok> <std> <ver> <rev> <rsvd>
		
		<std>			::=	<byte>	{Code identifying reference standard.  
								The codes are defined as follows:
							  	00H     = ANSI C12.18
							  	01H     = For use by Industry Canada
							 	02H-FFH = Reserved }
		
		<ver>			::=	<byte>	{Referenced standard version number}
		        
		<rev>			::=	<byte>	{Referenced standard revision number}
		
		<rsvd>			::=	<byte>	{Reserved for future use.  
									This byte shall be set to 00H until 
									further defined.}
	
	
	===================================
	=====	read
	===================================
	Read request:
		<read>			::=	<full_read> | <pread_index> | 
							<pread_offset> | <pread_default>
		
		<full_read>		::=	30H <tableid>
		
		<pread_index>	::=	<3jH> <tableid> <index>+ <count>
		
		<3jH>			::=	31H |	{ 1 <index> included in request }
							32H |	{ 2 <index> included in request }
							33H |	{ 3 <index> included in request }
							34H |	{ 4 <index> included in request }
							35H |	{ 5 <index> included in request }
							36H |	{ 6 <index> included in request }
							37H |	{ 7 <index> included in request }
							38H |	{ 8 <index> included in request }
							39H 	{ 9 <index> included in request }
		
		<pread_default>	::=	3EH	{ Transfer default table }
		
		<pread_offset>	::=	3FH <tableid> <offset> <count>
		
		<tableid>		::=	<word16>	{ Table identifier }
		
		<offset>		::=	<word24>	{ Offset into data table in bytes }
		
		<index>			::=	<word16>	{ Index value used to locate start of data }
		<count>			::=	<word16>	{ Length of table data requested, in bytes }
		
	Read response:
		<read_r>		::=	<nok> | <ok> <table_data>
		
		<table_data>	::=	<count> <data> <cksum>
		
		<count>			::=	<word16>	{ Length of <data> returned, in bytes }
		
		<data>			::=	<byte>+
		
		<cksum>			::=	<byte>	{ 2's compliment checksum computed only on 
									the <data> portion of <table_data>. 
									The checksum is computed by summing the bytes 
									(ignoring overflow) and negating the result}
		
	===================================
	=====	write
	===================================
	Write request:
		<write>			::=	<full_write> | <pwrite_index> | <pwrite_offset>
		
		<full_write>	::=	40H <tableid> <table_data>
		
		<pwrite_index>	::=	<4jH> <tableid> <index>+ <table_data>
		
		<4jH>			::=	41H |	{ 1 <index> included in request }
							42H |	{ 2 <index> included in request }
							43H |	{ 3 <index> included in request }
							44H |	{ 4 <index> included in request }
							45H |	{ 5 <index> included in request }
							46H |	{ 6 <index> included in request }
							47H |	{ 7 <index> included in request }
							48H |	{ 8 <index> included in request }
							49H 	{ 9 <index> included in request }
		
		<pwrite_offset>	::=	4FH <tableid> <offset> <table_data>
		
		<tableid>		::=	<word16>	{ Table identifier }
		
		<offset>		::=	<word24>	{ Offset into data table in bytes }
		
		<index>			::=	<word16>	{ Index value used to locate start of data }
		<table_data>	::=	<count> <data> <cksum>
		
		<count>			::=	<word16>	{ Length of <data> to be written, in bytes }
		
		<data>			::=	<byte>+
		
		<cksum>			::=	<byte>		{ 2's compliment checksum computed only
											on the <data> portion of 
											<table_data>.  The checksum is 
											computed by summing the bytes 
											(ignoring overflow) and negating 
											the result. }
	
		
	Write response:
		<write_r>		::=	<nok> | <ok>
		

	===================================
	=====	logon
	===================================
	Logon Request:
		<logon>			::=	50H <user_id> <user>
		
		<user_id>		::=	<word16>	{User identification code}
		
		<user>			::=	<byte>+10	{10 bytes containing user identification}
	
	
	Logon Response:
		<logon_r>		::=	<err> | <bsy> | <iar> |	<isss> | <ok>
	
	
	===================================
	=====	logoff
	===================================
	Logoff Request:
		<logoff>		::=	52H	
		
	Logoff Response:
		<logoff_r>		::=	<err> | <bsy> |	<isss> | <ok>
	
	===================================
	=====	negotiate
	===================================
	Negotiate Request:                                                  
		<negotiate>		::=	<6jH> <packet_size> <nbr_packet> <baud_rate>*
		
		<6jH>			::=	60H |	{ No <baud rate> included in request. 
										Stay at default baud rate }
							61H |	{ 1 <baud rate> included in request }
							62H |	{ 2 <baud rate> included in request }
							63H |	{ 3 <baud rate> included in request }
							64H |	{ 4 <baud rate> included in request }
							65H |	{ 5 <baud rate> included in request }
							66H |	{ 6 <baud rate> included in request }
							67H |	{ 7 <baud rate> included in request }
							68H |	{ 8 <baud rate> included in request }
							69H |	{ 9 <baud rate> included in request }
							6AH |	{10 <baud rate> included in request }
							6BH 	{11 <baud rate> included in request }
		
		<packet_size>	::=	<word16> { Maximum packet size supported, in 
											bytes. This value shall not be 
											greater than 8192 bytes.}
		
		<nbr_packet>	::=	<byte>	{ Maximum number of packets this layer 
										is able to reassemble into an upper 
										layer data structure at one time. }
		
		<baud_rate>		::=	00H |	{ Externally defined }
							01H |	{ 300 baud }
							02H |	{ 600 baud }
							03H |	{ 1200 baud }
							04H |	{ 2400 baud }
							05H |	{ 4800 baud }
							06H |	{ 9600 baud }
							07H |	{ 14400 baud }
							08H |	{ 19200 baud }
							09H |	{ 28800 baud }	
							0AH 	{ 56200 baud }
						{ 	0BH - FFH reserved }
					
	Negotiate Response:
		<negotiate_r>	::=	<err> | <sns> |	<bsy> |	<isss> |
							<ok> <packet_size> <nbr_packet> <baud_rate>

	===================================
	=====	wait
	===================================
	Wait Request:
		<wait>			::=	70H <time>	
	
		<time>			::=	<byte>	{ Suggested wait period in seconds. }
		
	Wait Response:
	
		<wait_r>		::=	<err> |	<sns> |	<bsy> |	<isss> | <ok> 
	
		     
	===================================
	=====	terminate
	===================================
	Terminate Request:
		<terminate>		::=	21H			     
	
	Terminate Response:
		<terminate_r>	::=	<err> |	<ok> 
	


--------------------------------------------------------------------------
**********************	 Revision History.   *****************************
--------------------------------------------------------------------------


*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "psem.h"
#include "se240mtr.h"
#include "diag.h"
#include "c1219.h"
#include "lputils.h"
#include "xmlgen.h"
#include "uartio.h"

#include "inifile.h"
#include "config.h"

#include "rest.h"

/*------------------------------------------------------------------------
**************************   local definitions	*************************
------------------------------------------------------------------------*/
// definitions for comm configuration for test

// define sizes of communications buffers
#define SIZERXBUF 8192 /*256*/
#define SIZETXBUF 256
#define NSIMPLEBYTES 10
#define SIZESIMPLE (sizeof(typePSEMHeader)+ NSIMPLEBYTES)

/*------------------------------------------------------------------------
*********************   prototypes this module		**********************
------------------------------------------------------------------------*/
int DoPSEMClientApp(void);
int UpdateTxState(int fd);
void UpdateRxState(int fd);
/*------------------------------------------------------------------------
************************  local Types and enums	**************************
------------------------------------------------------------------------*/

// types used to implement test scenarios for clients
typedef struct {
	uint8_t service;
	uint8_t data[1];
} typeTestMsg;

typedef struct {
	uint8_t * pData;
	uint16_t length;
	int (*pcb)(unsigned char *, int) ;	// callback routine addr or NULL
} typeScenario;

typedef enum {
	CLIENT_IDLE,
	CLIENT_REQUEST,
	CLIENT_WAITFORRESPONSE
} CLIENT_STATE;		


/*------------------------------------------------------------------------
*********************   macros local to this module	**********************
------------------------------------------------------------------------*/

#define SCENARIO_ENTRY(foo) {&(foo[0]),sizeof(foo), NULL},
#define SCENARIO_CB_ENTRY(foo, pcb) {&(foo[0]),sizeof(foo), pcb},
#define TESTMSG(name)  unsigned char  name[]
#define SCENARIO_MSG(foo) ((typeTestMsg *)(pScenarios[foo].pData))
#define RLP_SCENARIO_MSG(foo) ((typeTestMsg *)(LPScenarios[foo].pData))

/*------------------------------------------------------------------------
************************   Global to this module	**********************
------------------------------------------------------------------------*/

//-------------------------------------
// Transmit variables
//-------------------------------------
uint16_t	nTxBytes;	// count of bytes to transmit
uint8_t *	pTxData;	// pointer to current transmit data byte
int 		inTx;		// currently transmitting a message
uint8_t 	txAckNak;	// holds ack if waiting for transmit
int 		waitForAckNak;	// set to true after transmit to wait for ACK
uint8_t 	txState;	// state of TX state machine
time_t 		tmrWaitForAck;	// wait for ack timer
uint8_t 	txBuffer[SIZETXBUF];	// transmit buffer
int 		haveTxMsg;		// flag says txMsg waiting
int 		haveTxRetry;	// flag says retry waiting
int 		haveResponse;	// flag says valid response came in
uint16_t 	nTxMsg;		// number of bytes in simple message
uint8_t 	txToggle;	// transmit sequence number
uint8_t * 	lastTxMsg;// save for retry
uint16_t 	nLastTxMsg;// "
int			txFail;			// flag shows failure of tx packet sequence

static int (*pcallback)(unsigned char *, int ) ;

//-------------------------------------
// Receive variables
//-------------------------------------
uint16_t	nRxBytes;		// count of bytes received
uint8_t 	rxAckNak;		// holds ack if received
uint8_t 	rxState;		// state of RX state machine
uint8_t 	rxFCSLow;		// receive frame check sequence
uint8_t 	rxFCSHigh;		// receive frame check sequence
uint8_t 	rxToggle;		// last received sequence byte
uint8_t 	rxID;			// last identity byte
time_t 		tmrSinceLast;	// time since last received character
uint8_t 	rxBuffer[SIZERXBUF];	// receive buffer
int 		inRx;			// currently receiving a message
uint16_t 	nMsgBytes;		// number of msg bytes to receive
int 		gotChar;		// flag shows received char

//-------------------------------------
// Application layer state machine
//-------------------------------------
PSEM_STATE 		psemAppState;	// logical state of connection
time_t          tmrSession;		// session keep alive timer
uint16_t		retryCount;		// count for retries
time_t          tmrRetry;		// retry timer -- wait for response
PSEM_RESPONSE	responseCode;	// response code for last message
time_t			sessionTmrVal;	// session timer value

//-------------------------------------
// Other
//-------------------------------------
static time_t	ticCount;		// timer tic (1/10 sec) counter, diag purposes only
static uint64_t	start_time ;	// starticng clock value, msec

///////////////////////////////////////////////////////////////////////////////
//     Scenarios 
///////////////////////////////////////////////////////////////////////////////
// Delay Messages -- note: escape format to facilitate simulation
///////////////////////////////////////////////////////////////////////////////
TESTMSG(DELAY1)		= {0xff,0x01,0x00,0x00,0x00}; // 1 second delay
TESTMSG(DELAY5)		= {0xff,0x05,0x00,0x00,0x00}; // 5 second delay
TESTMSG(DELAY20) 	= {0xff,0x14,0x00,0x00,0x00}; // 20 second delay
TESTMSG(DELAY35) 	= {0xff,0x23,0x00,0x00,0x00}; // 35 second delay
TESTMSG(DELAY150) 	= {0xff,0x96,0x00,0x00,0x00}; // 150 second delay

///////////////////////////////////////////////////////////////////////////////
// Standard Messages
///////////////////////////////////////////////////////////////////////////////
TESTMSG(IDENTIFY) 	= { PSEM_IDENT};
TESTMSG(NEGOTIATE) 	= { PSEM_NEGOTIATE, (SIZERXBUF>>8), SIZERXBUF&0xff,  1};

////TESTMSG(LOGON) 		= { PSEM_LOGON, 0x12,0x34, 'M', 'A', 'R', 'T', 'Y', ' ', ' ', ' ', ' ', ' '};
TESTMSG(LOGON) 		= { PSEM_LOGON, 0x00,0x01,     'n', 'e', 't', 'z', 'e', 'r', 'o', ' ', ' ', ' '};
TESTMSG(LOGOFF) 	= { PSEM_LOGOFF};
TESTMSG(WAIT120) 		= { PSEM_WAIT, 120};
TESTMSG(WAIT60) 		= { PSEM_WAIT, 60};
TESTMSG(TERMINATE)	= { PSEM_TERMINATE};
//
TESTMSG(READTAB0) 		= { PSEM_READ, 0,0};
TESTMSG(READTAB3) 		= { PSEM_READ, 0,3};
//
TESTMSG(READTAB11) 		= { PSEM_READ, 0,11};
TESTMSG(READTAB12) 		= { PSEM_READ, 0,12};
TESTMSG(READTAB15) 		= { PSEM_READ, 0,15};
TESTMSG(READTAB16) 		= { PSEM_READ, 0,16};
//
TESTMSG(READTAB21) 		= { PSEM_READ, 0,21};
TESTMSG(READTAB22) 		= { PSEM_READ, 0,22};
TESTMSG(READTAB23) 		= { PSEM_READ, 0,23};
TESTMSG(READTAB27) 		= { PSEM_READ, 0,27};
TESTMSG(READTAB28) 		= { PSEM_READ, 0,28};
//
TESTMSG(READTAB61) 		= { PSEM_READ, 0,61};
TESTMSG(READTAB62) 		= { PSEM_READ, 0,62};
TESTMSG(READTAB63) 		= { PSEM_READ, 0,63};
TESTMSG(READTAB64_x) 		= { PSEM_READ_OFFSET, 0,64, 0, 0, 0, 0, 0 };	// read <blksize> bytes starting from <blkoffset>
TESTMSG(READLPCHUNK) 		= { PSEM_READ_OFFSET, 0,64, 0, 0, 0, 0, 0 };	// read next LP chunk from current LP block
TESTMSG(WRITE) 		= { PSEM_WRITE | 0x0f, 0,0x34, 0,0,0, 0,5, 0,0,0,0,0, 0x00};

////TESTMSG(SECURITY)	= { PSEM_SECURITY, 0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6,
////									   0xa6,0xa6,0xa6,0xa6, 0xa6,0xa6,0xa6,0xa6 } ;
TESTMSG(SECURITY)	= { PSEM_SECURITY,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0 } ;

TESTMSG(PROC84) = { PSEM_WRITE, 0x00, 0x07, 0x00, 0x03, 0x54, 0x08, 0x00, 0xA4} ;

// Build a test scenario by listing messages to send:
typeScenario IntroScenarios[] = {
	SCENARIO_ENTRY(IDENTIFY)
	SCENARIO_ENTRY(NEGOTIATE)
	SCENARIO_ENTRY(WAIT120)
	SCENARIO_ENTRY(LOGON)
		SCENARIO_ENTRY(DELAY1)
	SCENARIO_ENTRY(SECURITY)
	SCENARIO_ENTRY(WAIT120)
	SCENARIO_ENTRY(PROC84)
	SCENARIO_ENTRY(WAIT60)
	//
	SCENARIO_CB_ENTRY(READTAB0, gettab0)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB3, gettab3)
	SCENARIO_ENTRY(DELAY1)
	//
	SCENARIO_CB_ENTRY(READTAB11, gettab11)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB12, gettab12)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB15, gettab15)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB16, gettab16)
	SCENARIO_ENTRY(DELAY1)
	//
	SCENARIO_CB_ENTRY(READTAB21, gettab21)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB22, gettab22)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB23, gettab23)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB27, gettab27)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB28, gettab28)
	SCENARIO_ENTRY(DELAY1)
	//
	SCENARIO_CB_ENTRY(READTAB61, gettab61)
	SCENARIO_ENTRY(WAIT60)
	SCENARIO_CB_ENTRY(READTAB62, gettab62)
	SCENARIO_ENTRY(WAIT60)
	SCENARIO_CB_ENTRY(READTAB63, gettab63)
	SCENARIO_ENTRY(WAIT60)
} ;
#define NUM_INTRO_TEST_MSGS (sizeof(IntroScenarios)/sizeof(typeScenario))

////SCENARIO_CB_ENTRY(PROC84, gettab63)

typeScenario TrailerScenarios[] = {
//	SCENARIO_ENTRY(WRITE)
//		SCENARIO_ENTRY(DELAY1)
	SCENARIO_ENTRY(LOGOFF)
	SCENARIO_ENTRY(TERMINATE)
};                                                                    
#define NUM_TRAILER_TEST_MSGS (sizeof(TrailerScenarios)/sizeof(typeScenario))

typeScenario LPScenarios[] = {
		SCENARIO_CB_ENTRY(READTAB64_x, gettab64)
		SCENARIO_CB_ENTRY(READLPCHUNK, getlpchunk)
};
#define NUM_LP_READ_MSGS (sizeof(LPScenarios)/sizeof(typeScenario))

static volatile int readlpmode = 0 ;
static volatile readlp_info_t rlpinfo ;

typeScenario *pScenarios = NULL ;
int nScenarios = 0 ;

TESTMSG(READTABx) 		= { PSEM_READ, 0,0};
unsigned char *pReadTabMsgs = NULL ;

char *responseNames[] =
{
	"ACK",
	"NAK",
	"SNS",
	"ISC",
	"ONP",
	"IAR",
	"BSY",
	"DNR",
	"DLK",
	"RNO",
	"ISSS",
	"FAIL"
};


// Table for lookup version of crc computation
//; CCIT table = x^16 + x^15 + x^2 + 1
uint16_t          CCITShortTab[16] =
{
	0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xA50A, 0xB58B, 0xC60C, 0xD68D, 0xE70E, 0xF78F
};


//-------------------------------------


struct timeval tv0 ;

extern char seriodev[] ;
////extern char proc_dir[] ;
extern int baud;
extern char user[] ;
extern char pw[] ;

extern char tablist[] ;
extern int tables[] ;
extern int ntables ;

extern int verbose ;
extern int dosave ;
extern int doxml ;
extern int force_post ;

extern char outdir[] ;
extern char xmlfn[] ;

// Set Read LP parameters helper. Called from gettab63() and gettab64() callback functions
void set_rlp_params(int rlpmode, readlp_info_t *pinfo)
{
	readlpmode = rlpmode ;

	if (pinfo) {
		memcpy((char *)&rlpinfo, pinfo, sizeof(readlp_info_t)) ;
	}
	else {
		memset((char *)&rlpinfo, 0, sizeof(readlp_info_t)) ;
	}
	VVTRACE("==== next off=%d\n", rlpinfo.lpset_offset) ;
}

/*------------------------------------------------------------------------
************************   Protocol State Machines	**********************
------------------------------------------------------------------------*/

//------------------------------------------------------------------------
//	Kicks off send of message
//------------------------------------------------------------------------
//
void SendMsg(int fd, uint8_t *p, int n )
{
	int rc ;

	if (n > 0)	// must ensure > 0
	{
		// now we can transmit
		inTx = 1;

		if (verbose)
			DumpHex(p, n) ; fflush(stdout) ;

		rc = write(fd, p, n) ;
		if (rc > 0) {
			nTxBytes -= rc ;
			if (nTxBytes == 0) {
				inTx = 0;
			}
		}
	}
}

void SendAckNak(int fd, uint8_t ack)
{
	txAckNak 	= ack;
	pTxData 	= &txAckNak;
	nTxBytes 	= 1;
	////VVSHOWPACKET("Transmit ACK/NAK @", &txAckNak, 1);
	VTRACE("> %s\n", (ack) ? "ACK":"NAK") ;
	SendMsg(fd, pTxData, (int) nTxBytes);
}


//------------------------------------------------------------------------
//	int UpdateTxState()
//	updates the transmission of a message and/or ack/nak
//------------------------------------------------------------------------
//
int UpdateTxState(int fd)
{
	int rc = 1 ;

	// finished transmitting a character
	if (inTx) {
		// must ensure > 0
		if (nTxBytes > 0) {
			// now we can transmit
			SendMsg(fd, pTxData, (int) nTxBytes) ;
			if (nTxBytes)
				inTx = 0;
		}
		else
			inTx = 0;

	}

	switch (txState) {
 		case TX_IDLE:
			if (haveTxRetry)
			{
				// initialize retry send of message
				haveTxRetry	= 0;
				txState 	= TX_MSG;
			}
			else if (haveTxMsg) {
				// initialize send of message
				haveTxMsg 	= 0;
				retryCount 	= MAX_RETRY;
				nLastTxMsg 	= nTxMsg;
				lastTxMsg 	= txBuffer;
				txState 	= TX_MSG;
			}
			break;
 
		case TX_MSG:
			// check if the last TX is still pending -- might happen if
			// an ACK/NAK prior to sending response
			if(inTx==1)
				break;
					
			// start send of message
			nTxBytes = nLastTxMsg ;
			pTxData = lastTxMsg;

			SendMsg(fd, pTxData, (int) nTxBytes);
		
			waitForAckNak 	= 1;
			retryCount--;

			// go to wait for ack 
			txState = TX_WAITFORACK;
			SetN0MTimer(&tmrWaitForAck, WAITFORACK);

			if (verbose) {
				parse_request(&lastTxMsg[6], nLastTxMsg-6-2, verbose) ;
				DumpHex(lastTxMsg, nLastTxMsg);
			}
			break;

		case TX_WAITFORACK:
			// check if the last TX is still pending
			if(inTx==1)
				break;

			// Check if we received an ACK/NAK
			if (waitForAckNak == 0) {
				// process depending on ACK or NAK
				if(rxAckNak == PSEM_ACK) {
					txState 		= TX_WAITFORRESPONSE;
					SetN0MTimer(&tmrRetry,WAITFORRESPONSE);
					haveResponse 	= 0;
					txFail			= 0;
				}
				else {
					// if retryCount has tries left, resend or fail
					if (retryCount)
					{
						TRACE("Retrying transmit msg due to NAK: Try %d\n", MAX_RETRY - retryCount);
						txState = TX_IDLE;
						haveTxRetry	= 1;
					}
					else
						txState = TX_FAIL;
				}
			}
			else {
				// check for timeout
				if (CheckN0MTimer(&tmrWaitForAck)) {
					if (retryCount) {
						TRACE("Retrying transmit msg due to timeout waiting for ACK : Try %d\n", MAX_RETRY - retryCount);
						txState = TX_IDLE;	// try again
						haveTxRetry	= 1;
					}
					else
						txState = TX_FAIL;	// go to idle state
				}
			}
			break;

		case TX_WAITFORRESPONSE:
			// Check for response wait timeout
			if(CheckN0MTimer(&tmrRetry)) {
				// repsone didn't come in
				if(retryCount) {
					TRACE("Retrying transmit msg due to response timeout: Try %d\n", MAX_RETRY - retryCount);
					txState = TX_IDLE;	// retry send
					haveTxRetry	= 1;
				}
				else
					txState = TX_FAIL;
			}
			else {
				// check if response came in detected by receive state machine
				if(haveResponse == 1) {
					// Check if it was good response -- not duplicate and not bad
					if(txFail == 1) {
						if(txAckNak == PSEM_NAK) {
							// repsone didn't come in
							if(retryCount) {
								TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
								txState = TX_IDLE;	// retry send after sending NAK
							}
							else
								txState = TX_FAIL;
						}
						else {
							// it was duplicate, continue waiting
							haveResponse = 0;
						}
					}
					else
						txState = TX_IDLE;
				}
			}
			break;

		case TX_FAIL:
			// gave up trying to send message
			txState 		= TX_IDLE;
			psemAppState 	= PSEM_APP_BASE;
			responseCode 	= PSEM_RESPONSE_FAIL;
			haveResponse 	= 1;
			TRACE("TxMessage Failure @%d\n",(int)ticCount);
			rc = 0 ;
			break;
			
		default:
			// catch fault -- should not get invalid state
			// Show error and reinitialize comm
			TRACE("Unknown TX service @%d\n",(int)ticCount);
			InitializeComm(INITIALIZE_TX);
			rc = 0 ;
			break;
	}

	return rc ;
}

//------------------------------------------------------------------------
//	void UpdateRxState()
//	updates the reception of a message and/or ack/nak
//------------------------------------------------------------------------
//
void UpdateRxState(int fd)
{
	uint16_t          fcs;
	uint16_t          i;
////////////////////
	unsigned char c ;
	int n ;
	
	gotChar = 0;

	n = 0 ;
	if (uartio_dataready(fd)) {
		uartio_read(fd, (char *) &c) ;
		n = 1 ;
	}

	if (n == 1) {
		// check for receive of start of frame
		if (!inRx) {
			// framing looks for start of packet or ack/nak
			if (c == PSEM_STP) {
				inRx = 1;
			}
			else if (waitForAckNak) {
				// if waiting for acks see if we got it
				//  Note: skip over any garbage
				if( (c == PSEM_ACK) || (c == PSEM_NAK) ) {
					rxAckNak = c;
					waitForAckNak = 0;
				}
			}

			nRxBytes = 0;
		}

		// if count of bytes received less than max
		if (nRxBytes < SIZERXBUF - 1) {
			// store it in receive buffer
			rxBuffer[nRxBytes++] = c;
			gotChar = 1;
		}
	}
////////////////////
	// update received character timer if necessary
	if (gotChar == 1) {
		gotChar = 0;
		
		// set timer to detect timeout for framing
		SetN0MTimer(&tmrSinceLast, INTER_CHARACTER_TIME_OUT);
	}
	
	// update application layer state message timeout if not in idle
	if((psemAppState != PSEM_APP_BASE) && (CheckN0MTimer(&tmrSession)) ) {
		TRACE("Session timeout @%d\n",(int)ticCount);
		psemAppState = PSEM_APP_BASE;
		rxToggle = PSEM_CTRL_TGL;
	}
	
	
	// update receive state machine for data link layer
	switch (rxState) {
		case RX_IDLE:
			// check for in receive of packet
			if (inRx == 1) {
				// we are change state
				rxState = RX_HEADER;
			}
			break;

		case RX_HEADER:
			// Check for message done
			if (nRxBytes >= sizeof(typePSEMHeader)) {
				// compute length of message 
				nMsgBytes = sizeof(typePSEMHeader) - 1
					+ (rxBuffer[offsetof(typePSEMHeader, lengthmsb)] << 8)	// apdu
					+ rxBuffer[offsetof(typePSEMHeader, lengthmsb) + 1]
					+ 2;		// checksum

				// we are change state
				rxState = RX_BODY;
			}
			else if (CheckN0MTimer(&tmrSinceLast)) {
				inRx 	= 0;
				rxState = RX_IDLE;
			}
			break;

		case RX_BODY:
			// Check for message done
			if (nRxBytes >= nMsgBytes) {
				// we have a message
				////VVSHOWPACKET("Received message @", rxBuffer, nRxBytes);

				// check fcs
				CRCINITIALIZE(fcs);
				for (i = 0; i < nMsgBytes; i++)
					CRCUPDATECHAR(fcs, rxBuffer[i]);
				CRCFINALIZE(fcs);
				if (fcs == GOODFCS) {
					int cnt ;

					if (pcallback) {
						(pcallback)(&rxBuffer[6], nRxBytes-6-2) ;
						pcallback = NULL ;
					}

					cnt = parse_response(&rxBuffer[6], nRxBytes-6-2, (int) (((typePSEMHeader *)&txBuffer[0])->service), verbose) ;
					{
						int svc ;
						int tableid ;

						svc = (int) (((typePSEMHeader *)&txBuffer[0])->service) ;
						if ((svc == REQ_READ) || (svc == REQ_PREAD_OFF)) {
							int msglen ;

							msglen = (int) (((typePSEMHeader *)&txBuffer[0])->lengthmsb) ;
							msglen = (msglen<<8) + (int) (((typePSEMHeader *)&txBuffer[0])->lengthlsb) ;
							tableid = txBuffer[7] ;
							tableid = (tableid<<8) + txBuffer[8] ;
							if ((dosave != 0) /* || (tableid == 64)*/)
								savetable(tableid, &rxBuffer[6+3], cnt /*nRxBytes-6-2*/) ;
						}
					}

					// good crc, ack it
					SendAckNak(fd, PSEM_ACK);

					// process APDU
					// check for duplicate if same as last sequence number and same CRC
					if( (   rxToggle 	== (PSEM_CTRL_TGL & rxBuffer[offsetof(typePSEMHeader,ctrl)]) )
						&& (rxFCSHigh 	== rxBuffer[nMsgBytes-2])
						&& (rxFCSLow 	== rxBuffer[nMsgBytes-1])
						&& (rxID 		== rxBuffer[offsetof(typePSEMHeader,fill)]) ) {
						// duplicate! we acked it, now back to idle
						TRACE("Received duplicate @%d\n",(int)ticCount);
						inRx 			= 0;
						rxState 		= RX_IDLE;
 						break;
					}
					else {
						if(txState != TX_WAITFORRESPONSE) {
							TRACE("Received unexpected packet @%d\n",(int)ticCount);
							inRx 			= 0;
							rxState 		= RX_IDLE;
	 						break;
						}
						else {
							// Its a good new packet, we can try to process at application layer
							// update for next time
							rxToggle 	= (uint8_t)(PSEM_CTRL_TGL & rxBuffer[offsetof(typePSEMHeader,ctrl)]);
							rxFCSHigh 	= rxBuffer[nMsgBytes-2];
							rxFCSLow 	= rxBuffer[nMsgBytes-1];
							rxID		= rxBuffer[offsetof(typePSEMHeader,fill)];
							
							// process APDU
							rxState = RX_PROCESS;
						}
					}					
				}
				else {
					// its a bad packet or fragment -- NAK it
					TRACE("Received bad checksum @%d\n",(int)ticCount);
					TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
					SendAckNak(fd, PSEM_NAK);

					if(retryCount == 0) {
						haveResponse 	= 1;
						txFail			= 1;
					}

					// back to idle
					inRx 			= 0;
					rxState 		= RX_IDLE;
				}
			}
			else {
				// see if we waited too long
				if (CheckN0MTimer(&tmrSinceLast)) {
					// timed out waiting for end of message
					TRACE("Timed out due to receive of fragment @%d\n",(int)ticCount);
					TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
					SendAckNak(fd, PSEM_NAK);

					if(retryCount == 0) {
						haveResponse 	= 1;
						txFail			= 1;
					}

					inRx 			= 0;
					rxState 		= RX_IDLE;
				}
			}
			break;

		case RX_PROCESS:
			// try to process message
			haveResponse = 1;
			if (DoPSEMClientApp()) {
				// timed out waiting for end of message
				inRx 	= 0;		// free up receive buffer
				rxState = RX_IDLE;	// return to idle
			}
			break;

		default:
			// Show error and reinitialize comm
			TRACE("Unknown RX service @%d\n",(int)ticCount);
			InitializeComm(INITIALIZE_RX);
			break;
	}
}

//------------------------------------------------------------------------
//	int DoPSEMClientApp()
//	Processes an application layer message.  Message is to be handled, 
//	  providing response buffer is available. 
//		if buffer is not available, 
//	  		we will wait, return 0
// 		if successfully complete on message, return 1
//------------------------------------------------------------------------
//
int DoPSEMClientApp()
{
	uint8_t 			service;		// current app service invoked
	
	// initialize result code
	responseCode = rxBuffer[offsetof(typePSEMHeader, service)];
	service =  ((typePSEMHeader *)&txBuffer[0])->service;
    
	// handle service request
	switch (service) {
		case PSEM_IDENT:
			////VTRACE("> Ident -- %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_ID;
			break;

		case PSEM_TERMINATE:
			////VTRACE("> Terminate -- %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_BASE;
			sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;
			break;			

		case PSEM_READ_OFFSET:
		case PSEM_READ:
			////VTRACE("> Read -- %s\n",responseNames[responseCode]);
			break;			

		case PSEM_WRITE:
		case PSEM_WRITE_OFFSET:
			////VTRACE("> Write -- %s\n",responseNames[responseCode]);
			break;

		case PSEM_LOGON:
			////VTRACE("> Logon -- %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_SESSION;
			break;
		
		case PSEM_SECURITY:
			////VTRACE("> Security -- %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_SESSION;
			break;

		case PSEM_LOGOFF:
			////VTRACE("> Logoff -- %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_ID;
			sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;
			break;
			
		case PSEM_NEGOTIATE:
			////VTRACE("> Negotiate -- %s\n",responseNames[responseCode]);
			break;
			
		case PSEM_WAIT:
			////VTRACE("> Wait -- %s\n",responseNames[responseCode]);
			// update our session timer value
			sessionTmrVal = (time_t)txBuffer[offsetof(typePSEMHeader, service)+1]*(time_t)10;
			break;
			
		default:
			// use simple response -- service not supported
			TRACE("> Processed unkown response packet: %s\n",responseNames[responseCode]);
			break;
	}


    // We processed a message, should update Session timer
	SetN0MTimer(&tmrSession, sessionTmrVal);
	
	return (1);
}



//------------------------------------------------------------------------
// uint16_t                           // returns length of packet
// SetTxBuffer(PSEM_FRAME_FMT fmt, 	// the format of the frame 
// uint8_t servicecode, 				// a service/response code
// uint8_t * data, 				// data for the frame
// uint16_t length, 					// length of data
// uint8_t * buffer, 					// buffer to place packet in
// uint16_t sizebuffer)				// size of destination buffer
//
//	SetTxBuffer is the packet assembly method for the protocol.  Based on 
// 	 what is passed to it, it writes a packet frame in transmit buffer to 
//	 send.
//------------------------------------------------------------------------
//
// returns length of packet
uint16_t SetTxBuffer(PSEM_FRAME_FMT fmt, 	// the format of the frame
		uint8_t servicecode,	// a service / response code
		uint8_t * data, 		// data for the frame
		uint16_t length, 		// length of data
		uint8_t * buffer, 		// buffer to place packet in
		uint16_t sizebuffer)	// size of destination buffer
{
	uint16_t	fcs;	// for frame check sequence
	uint16_t	i;
	uint8_t *	pT;
	uint8_t		csum;
	uint16_t	msgLength;	// the length of message + service

	// check if too large
	if (length >= sizebuffer) {
		TRACE("Error tried to overrun transmit buffer @%d\n",(int)ticCount);
		return(0);
	}

	// check if this is request or PSEM response with data
	if (fmt == PSEM_FRAME_RESPONSE)
		msgLength = length + 4;
	else
		msgLength = length + 1;

	// build the header
	pT = buffer;
	*pT++ = PSEM_STP;			// start flag PSEM_STP
	*pT++ = 0;					// reserved for MFG, default 0
	txToggle ^= PSEM_CTRL_TGL;	// toggle sequence bit
	*pT++ = txToggle;			// Set control byte
 
	*pT++ = 0;					// segmented sequence number always 0
	*pT++ = (uint8_t)(msgLength >> 8);		// length of message in bytes
	*pT++ = (uint8_t)(msgLength & 0xff);	// length of message in bytes
	*pT++ = servicecode;		// PSEM service

	// check if this is request or PSEM response with data
	if (fmt == PSEM_FRAME_RESPONSE) {
		// its response with data = <count> <data> <cksum>
		*pT++ = (uint8_t)(length >> 8);
		*pT++ = (uint8_t)(length & 0xff);
		csum = 0;

		// copy in data and compute checksum
		for (i = 0; i < length; i++) {
			csum += data[i];
			*pT++ = data[i];
		}

		// place response data checksum in packet
		*pT++ = (uint8_t)(~csum + 1);		// negate
	}
	else {
		// copy in data and compute checksum
		for (i = 0; i < length; i++)
			*pT++ = data[i];
	}
	
	// compute checksum
	msgLength = ((uint16_t) (pT - buffer));
	pT = buffer;
	CRCINITIALIZE(fcs);
	for (i = 0; i < msgLength; i++) {
		CRCUPDATECHAR(fcs, *pT);
		pT++;
	}

	fcs = ~fcs;
	*pT++ = (uint8_t)(fcs & 0xff);
	*pT++ = (uint8_t)(fcs >> 8);

	// return total length of message
	return ((uint16_t) (pT - buffer));
}

/*------------------------------------------------------------------------
*********************   Communications Initialization	******************
------------------------------------------------------------------------*/

//------------------------------------------------------------------------
// void InitializeComm(INITIALIZE_MYPLATFORM what)
//		Initializes communications based on flags that determine
//		 if transmit, receive, or communications interface are to
//		 be initialized.
//------------------------------------------------------------------------
//
int InitializeComm(INITIALIZE_MYPLATFORM what)
{
	int rc = -1 ;

	// Always initialize state of connection
	psemAppState = PSEM_APP_BASE;		// logical state of connection


	tmrSession = 0;				// time since last received character
	sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;

	// initialize receive state machine
	if (what & INITIALIZE_RX) {
		nRxBytes 	= 0;
		inRx 		= 0;
		rxAckNak 	= 0;
		rxState 	= RX_IDLE;
		rxFCSLow 	= INITFCS & 0xff;
		rxFCSHigh 	= INITFCS >> 8;
		rxID		= 0xff;
		tmrSinceLast= 0;
		nMsgBytes 	= 0;
		rxToggle 	= PSEM_CTRL_TGL;
		gotChar		= 0;
		memset(rxBuffer, 0, SIZERXBUF);
	}

	// initialize transmit state machine
	if (what & INITIALIZE_TX) {
		nTxBytes 	= 0;
		pTxData 	= NULL;
		inTx 		= 0;
		txAckNak 	= 0;
		waitForAckNak = 0;
		txState 	= TX_IDLE;
		tmrWaitForAck = 0;
		haveTxMsg 	= 0;
		haveTxRetry = 0;
		nTxMsg 		= 0;
		txToggle 	= PSEM_CTRL_TGL;
		lastTxMsg 	= NULL;
		nLastTxMsg 	= 0;
		memset(txBuffer, 0, SIZETXBUF);
	}


	if (what & INITIALIZE_COMM) {
		rc = uartio_open(seriodev, baud, 'N', 8, 1, 0) ;
	}

	return rc ;
}

//------------------------------------------------------------------------=
// void TerminateComm( )
//	Terminate -- probably only useful for PC simulation to restore ports
//------------------------------------------------------------------------=
//
void TerminateComm(int fd)
{
	uartio_close(fd) ;
}


/*------------------------------------------------------------------------
**************************   Timer and timing support	******************
------------------------------------------------------------------------*/

// returns current monotonic time in milliseconds, 0 on error
//
uint64_t get_sys_timer()
{
	struct timespec ts ;
	uint64_t tt = 0 ;
	clockid_t clk_id ;
#ifdef __CYGWIN__
	clk_id = CLOCK_MONOTONIC ;
#else
	clk_id = CLOCK_MONOTONIC_RAW ;
#endif

	if (clock_gettime(clk_id,&ts) == 0) {
		tt = ts.tv_sec ;
		tt = (tt*1000) + (ts.tv_nsec / 1000000) ;
	}

	return tt ;
}


//------------------------------------------------------------------------
//	void InitializePCTimer()
//	Sets up timer tic interrupt handler
//------------------------------------------------------------------------
//
void InitializeN0MTimer()
{
	start_time = get_sys_timer() ;
	ticCount = 0;

}

//------------------------------------------------------------------------
//	void TerminatePCTimer()
//	Removes timer tic interrupt handler
//------------------------------------------------------------------------
//
void TerminateN0MTimer()
{
}

//------------------------------------------------------------------------
//	void SetTimer()
//	Sets a timer, delt is specified in 1/10 of second
//------------------------------------------------------------------------
//
void SetN0MTimer(time_t * t, time_t delt)
{
	uint64_t tt ;

	if ((tt = get_sys_timer()) != 0) {
		ticCount = (time_t) ((tt - start_time)/100) ;
		*t = ticCount + delt ;
		// make sure not 0 Note: will cause 1 tic error at 0
		if (*t == 0)
			(*t)++;
	}
}

//------------------------------------------------------------------------
//	void CheckTimer()
//	Sets a timer
//------------------------------------------------------------------------
//
int CheckN0MTimer(time_t * t)
{
	uint64_t tt = get_sys_timer() ;
	int rc = 1 ; // expired, by default
	time_t tc ;

	if (tt != 0) {
		tc = (time_t) ((tt - start_time)/100) ;
		if (tc >= (*t)) {
			ticCount = tc ;
			*t = 0 ;
			rc = 1 ;
		}
		else {
			rc = 0 ;
		}
	}
	return rc ;

}



/*------------------------------------------------------------------------
************************   	Main program		**************************
------------------------------------------------------------------------*/

int kbhit(int fd)
{
	return 0;
}

int main(int argc, char ** argv)
{
	time_t  tmrSimulation;
	int messageCnt;
	int done = 0;
	CLIENT_STATE clientState;
	int i, j, tmptab ;
	char *saveptr ;
	char *p ;
	int fd ;
	int rc = 0 ;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (!cnf_init(argc, argv)) {
		return 1 ;
	}

	if (post_file[0] != '\0') {
		char *post ;
		int fsize = 0 ;

		post = get_xmlbuf(post_file, &fsize) ;

		if (post) {
			if (fsize <= 0) {
				fsize = strlen(post) ;
			}
			do_restful_post(post_url, post_bearer, post, fsize, checkpeer_flag, checkhostname_flag) ;
			free_xmlbuf(post) ;
			return 0 ;
		}
		else
			return 1 ;
	}


	i = strlen(user) ;
	j = sizeof(LOGON) ;
	memcpy(&LOGON[3], user, i) ;
	i += 3 ;
	while (i<j) {
		LOGON[i++] = ' ' ;
	}

	{
		extern int pw_size ;
		typeScenario *ps ;

		memcpy(&SECURITY[1], &pw[0], pw_size) ;

		for (ps = &IntroScenarios[0] ; ps < &IntroScenarios[NUM_INTRO_TEST_MSGS]; ps++) {
			if (ps->pData == ((uint8_t *)&SECURITY)) {
				ps->length = pw_size + 1 ;
				break ;
			}
		}
	}

	if (strlen(tablist) > 0) {
		p = strtok_r(&tablist[0], ",", &saveptr) ;
		for (j=0; j < 128; j++) {
			if (p==NULL)
				break ;
			tmptab = 0;
			tmptab = atoi(p) ;
			if ((tmptab>0) && (tmptab < 100)) {
				if ((tmptab <60) && (tmptab >64)) {
					tables[ntables++] = tmptab ;
				}
			}
			p = strtok_r(NULL, ",", &saveptr) ;
		}
		if (ntables > 0) {
			unsigned char *pmsg ;

			pReadTabMsgs = malloc(ntables * sizeof(READTABx)) ;
			if (pReadTabMsgs) {
				for (j=0; j < ntables; j++) {
					pmsg = pReadTabMsgs+j*sizeof(READTABx) ;
					memcpy(pmsg, &READTABx, sizeof(READTABx)) ;
					pmsg[1] = tables[j] >> 8 ;
					pmsg[2] = tables[j] & 0xFF ;
				}
			}
			else
				ntables = 0 ;
		}
	}

	nScenarios = NUM_INTRO_TEST_MSGS + ntables + NUM_TRAILER_TEST_MSGS ;
	pScenarios = malloc(nScenarios * sizeof(typeScenario)) ;
	if (pScenarios == NULL) {
		TRACE("Can't allocate memory for %d scenarios\n", nScenarios) ;
		return 1 ;
	}

	memcpy(pScenarios, &IntroScenarios, NUM_INTRO_TEST_MSGS * sizeof(typeScenario)) ;
	for (i=0; i < ntables; i++) {
		pScenarios[NUM_INTRO_TEST_MSGS+i].pData = pReadTabMsgs+i*sizeof(READTABx) ;
		pScenarios[NUM_INTRO_TEST_MSGS+i].length = sizeof(READTABx) ;
	}
	memcpy(pScenarios + NUM_INTRO_TEST_MSGS + ntables, &TrailerScenarios, NUM_TRAILER_TEST_MSGS * sizeof(typeScenario)) ;

	// initialize communications processes and timing interrupts
	fd = InitializeComm(INITIALIZE_RX | INITIALIZE_TX | INITIALIZE_COMM);
	if (fd <0) {
		free(pScenarios) ;
		printf("*** cannot open %s\n", seriodev) ;
		return 1 ;
	}

	InitializeN0MTimer();
	
	// Initialize client simulation
	SetN0MTimer(&tmrSimulation, 10);	// set initial 1 second delay
	clientState 	= CLIENT_IDLE;
	messageCnt 		= 0;

	lputils_init() ;

	if (dosave) {
		dirunlink(&outdir[0], "tab064.dat") ;
	}

	//=================================
	// Forever Loop
	//=================================
	//
	// Hit any key to exit

	while (!kbhit(fd) && !done) {
		// Update transmit state machine
		rc = UpdateTxState(fd);
		if (rc == 0) {
			rc = 1 ;
			done = 1 ;
			continue ;
		}

		// Update receive state machine
		UpdateRxState(fd);
		
		// process any waiting messages
		switch(clientState) {
			case CLIENT_IDLE:
				if(CheckN0MTimer(&tmrSimulation))
					clientState = CLIENT_REQUEST;
				break;
			case CLIENT_REQUEST:
				// Check for messages to send
				if(messageCnt < nScenarios) {
					// start new message
					if( SCENARIO_MSG(messageCnt)->service == 0xff ) {
						// This is internal timer message
						SetN0MTimer(&tmrSimulation,
							( ((SCENARIO_MSG(messageCnt)->data[0])
							+ (SCENARIO_MSG(messageCnt)->data[1]<<8)) 
							* 10) );
						clientState = CLIENT_IDLE;
					}
					else {
						// This is PSEM message from script
						if (readlpmode) {
							unsigned int blkoffset ;
							unsigned char *p ;
							int chunksz ;

							p = LPScenarios[0].pData ;
							chunksz = rlpinfo.blksize ;
							if (rlpinfo.nread > 0) {
								chunksz = rlpinfo.blksize - rlpinfo.nread ;
								// shouldn't happen
								if (chunksz < 0)
									chunksz = 0 ;
							}

							blkoffset = rlpinfo.lpset_offset ;
							p[3] = (blkoffset >> 16) & 0xff ;
							p[4] = (blkoffset >> 8) & 0xff ;
							p[5] = blkoffset & 0xff ;
							p[6] = (chunksz >> 8) & 0xff ;
							p[7] = chunksz & 0xff ;

							nTxMsg = SetTxBuffer(
								PSEM_FRAME_SERVICE,
								(uint8_t) (RLP_SCENARIO_MSG(0)->service),
								(uint8_t *)(&RLP_SCENARIO_MSG(0)->data[0]),
								LPScenarios[0].length-1,
								txBuffer,
								SIZETXBUF);
							pcallback = LPScenarios[0].pcb ;
						}
						else {
							nTxMsg = SetTxBuffer(
									PSEM_FRAME_SERVICE,
									(uint8_t) (SCENARIO_MSG(messageCnt)->service),
									(uint8_t *)(&SCENARIO_MSG(messageCnt)->data[0]),
									pScenarios[messageCnt].length-1,
									txBuffer,
									SIZETXBUF);
							pcallback = pScenarios[messageCnt].pcb  ;
						}
						clientState 		= CLIENT_WAITFORRESPONSE;
						haveTxMsg 		= 1;
						haveResponse	= 0;
					}
					
					// stay on the same 'read LP block' message while in readLP mode
					if (readlpmode == 0)
						messageCnt++;
				}
				else {
					// Done simulation
					done = 1;
					rc = 0 ;
				}
				break;
				
				
			case CLIENT_WAITFORRESPONSE:
				// response is done when haveResponse flag set && ack has
				//	been sent
				if( (haveResponse) 
					&& (!haveTxRetry)
					&& (txState == CLIENT_IDLE) )
						clientState = CLIENT_REQUEST;	// return to request
				break;
		}
	}

	// clean up for PC exit
	TerminateN0MTimer();
	TerminateComm(fd);

	if (rc == 0)
	{
		if (dosave == 0) {		// save the LP table with a special name
			unsigned int t0, t1, dur ;
			meterinfo_t mi ;
			readlp_info_t rlpi ;
			char *lpaddr ;
			int lplen ;
			char lpfn[64] ;
			FILE *fp ;

			t0 = lp_getstart(0) ;
			dur = lp_getduration(0) ;
			lp_getmeterinfo(0, &mi, &rlpi) ;
			t1 = t0 + dur - (mi.intvlen*60) ;
			lpaddr = lp_get_start_addr(0) ;
			lplen = lp_get_tot_size(0) ;

			printf("*** start=%u, end=%u, dur=%u, addr=%08X, lpsize=%d\n",
				t0, t1, dur, (unsigned int) lpaddr, lplen) ;

			sprintf(&lpfn[0], "lp_%010u-%010u.dat", t0, t1) ;
			if ((fp = fdiropen(&outdir[0], &lpfn[0], "wb")) != NULL) {
				rc = fwrite(lpaddr, lplen, 1, fp) ;
				fclose(fp) ;
				// fwrite should return the number of items written, i.e. '1' on success
				if (rc == 1) {
					rc = 0 ;
				}
				else {
					TRACE("*** Cannot write %d bytes to %s/%s\n", lplen, outdir, lpfn) ;
				}
			}
			else {
				rc = 1 ;
			}
		}
	}

	if ((rc == 0) && (doxml != 0)) {
		extern void lp_makexml(char *pth, char *xmlfn) ;

		lp_makexml(&outdir[0], xmlfn) ; // xmlfn array gets generated xml file name

		//
		strcat(&outdir[0], "/") ;
		strcat(&outdir[0], xmlfn) ;
		if (force_post) {
			char *post ;
			int fsize = 0 ;

			post = get_xmlbuf(outdir, &fsize) ;

			if (post) {
				if (fsize <= 0 ) {
					fsize = strlen(post) ;
				}
				do_restful_post(post_url, post_bearer, post, fsize, checkpeer_flag, checkhostname_flag) ;
				free_xmlbuf(post) ;
			}
		}

		if (doxml == 0) { // requested to do not save the resulting XML file
			unlink(outdir) ;
		}
	}

	lputils_deinit() ;

	return rc ;
}
