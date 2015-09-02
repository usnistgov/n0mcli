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
	#define __WIN32__      Define to include WIN32API specific code (including main())


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
	<RESERVED>		::= <BYTE>	Reserved for manufacturer or utility use.(Default 0x00)
	<CTRL>			::= <BYTE>	Control field:
							Bit 7. If true packet is part of segmented transmission.
							Bit 6. If true then first of a multi-packet transmission.
							Bit 5. Represents a toggle bit to reject duplicate packets.
								This bit is toggled for each new packet sent.
								Retransmitted packets keep the same state
							Bits 0 to 4, Reserved

	<SEQ_NBR>		::= <BYTE>	Number that is decremented by one for each new packet sent.
	
	<LENGTH>		::= <WORD16>Number of bytes of data in packet

	<DATA>			::= <BYTE>+	<length> number of bytes of actual packet data.(MAX 8138)

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

#ifdef __WIN32__
#include <sys/time.h>
#include "serio.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "customtypes.h"
#include "psem.h"
#include "SE240mtr.h"
#include "diag.h"
#include "c1219.h"
#include "lputils.h"
#include "xmlgen.h"

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
BOOL DoPSEMClientApp(void);
void UpdateTxState(void);
void UpdateRxState(void);
/*------------------------------------------------------------------------
************************  local Types and enums	**************************
------------------------------------------------------------------------*/

// types used to implement test scenarios for clients
typedef struct {
	BYTE service;
	BYTE data[1];
} typeTestMsg;

typedef struct {
	BYTE * pData;
	UINT16 length;
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
UINT16          nTxBytes;		// count of bytes to transmit
PBYTE           pTxData;		// pointer to current transmit data byte 
BOOL            inTx;			// currently transmitting a message
BYTE            txAckNak;		// holds ack if waiting for transmit
BOOL            waitForAckNak;	// set to true after transmit to wait for ACK
BYTE            txState;		// state of TX state machine
TIME_T          tmrWaitForAck;	// wait for ack timer
BYTE            txBuffer[SIZETXBUF];// transmit buffer
BOOL            haveTxMsg;		// flag says txMsg waiting
BOOL            haveTxRetry;	// flag says retry waiting
BOOL			haveResponse;	// flag says valid response came in
UINT16          nTxMsg;			// number of bytes in simple message
BYTE            txToggle;		// transmit sequence number
PBYTE           lastTxMsg;		// save for retry
UINT16          nLastTxMsg;		// "
BOOL			txFail;			// flag shows failure of tx packet sequence
static int (*pcallback)(unsigned char *, int ) ;

//-------------------------------------
// Receive variables
//-------------------------------------
UINT16          nRxBytes;		// count of bytes received
BYTE            rxAckNak;		// holds ack if received
BYTE            rxState;		// state of RX state machine
BYTE 			rxFCSLow;		// receive frame check sequence
BYTE 			rxFCSHigh;		// receive frame check sequence
BYTE            rxToggle;		// last received sequence byte
BYTE			rxID;			// last identity byte
TIME_T          tmrSinceLast;	// time since last received character
BYTE            rxBuffer[SIZERXBUF];// receive buffer
BOOL            inRx;			// currently receiving a message
UINT16          nMsgBytes;		// number of msg bytes to receive
BOOL			gotChar;		// flag shows received char

//-------------------------------------
// Application layer state machine
//-------------------------------------
PSEM_STATE 		psemAppState;	// logical state of connection
TIME_T          tmrSession;		// session keep alive timer
UINT16			retryCount;		// count for retries
TIME_T          tmrRetry;		// retry timer -- wait for response
PSEM_RESPONSE	responseCode;	// response code for last message
TIME_T			sessionTmrVal;	// session timer value

//-------------------------------------
// Other
//-------------------------------------
TIME_T          ticCount;		// timer interrupt tic counter

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

TESTMSG(LOGON) 		= { PSEM_LOGON, 0x12,0x34, 'M', 'A', 'R', 'T', 'Y', ' ', ' ', ' ', ' ', ' '};
TESTMSG(LOGOFF) 	= { PSEM_LOGOFF};
TESTMSG(WAIT120) 		= { PSEM_WAIT, 120};
TESTMSG(WAIT60) 		= { PSEM_WAIT, 60};
TESTMSG(TERMINATE)	= { PSEM_TERMINATE};
TESTMSG(READTAB0) 		= { PSEM_READ, 0,0};
TESTMSG(READTAB61) 		= { PSEM_READ, 0,61};
TESTMSG(READTAB62) 		= { PSEM_READ, 0,62};
TESTMSG(READTAB63) 		= { PSEM_READ, 0,63};
TESTMSG(READTAB64_x) 		= { PSEM_READ_OFFSET, 0,64, 0, 0, 0, 0, 0 };	// read <blksize> bytes starting from <blkoffset>
TESTMSG(WRITE) 		= { PSEM_WRITE | 0x0f, 0,0x34, 0,0,0, 0,5, 0,0,0,0,0, 0x00};
TESTMSG(SECURITY)	= { PSEM_SECURITY, '0','0','0','0','0', '0','0','0','0','0', '0','0','0','0','0', '0','0','0','0','0' } ;

// Build a test scenario by listing messages to send:
typeScenario IntroScenarios[] = {
	SCENARIO_ENTRY(IDENTIFY)
	SCENARIO_ENTRY(NEGOTIATE)
	SCENARIO_ENTRY(WAIT120)
	SCENARIO_ENTRY(LOGON)
		SCENARIO_ENTRY(DELAY1)
	SCENARIO_ENTRY(SECURITY)
	SCENARIO_ENTRY(WAIT120)
	SCENARIO_CB_ENTRY(READTAB0, gettab0)
	SCENARIO_ENTRY(DELAY1)
	SCENARIO_CB_ENTRY(READTAB61, gettab61)
	SCENARIO_ENTRY(WAIT60)
	SCENARIO_CB_ENTRY(READTAB62, gettab62)
	SCENARIO_ENTRY(WAIT60)
	SCENARIO_CB_ENTRY(READTAB63, gettab63)
	SCENARIO_ENTRY(WAIT60)
} ;
#define NUM_INTRO_TEST_MSGS (sizeof(IntroScenarios)/sizeof(typeScenario))

typeScenario TrailerScenarios[] = {
//	SCENARIO_ENTRY(WRITE)
//		SCENARIO_ENTRY(DELAY1)
	SCENARIO_ENTRY(LOGOFF)
	SCENARIO_ENTRY(TERMINATE)
};                                                                    
#define NUM_TRAILER_TEST_MSGS (sizeof(TrailerScenarios)/sizeof(typeScenario))

typeScenario LPScenarios[] = {
		SCENARIO_CB_ENTRY(READTAB64_x, gettab64)
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
	"PSEM_RESPONSE_OK",
	"PSEM_RESPONSE_ERR",
	"PSEM_RESPONSE_SNS",
	"PSEM_RESPONSE_ISC",
	"PSEM_RESPONSE_ONP",
	"PSEM_RESPONSE_IAR",
	"PSEM_RESPONSE_BSY",
	"PSEM_RESPONSE_DNR",
	"PSEM_RESPONSE_DLK",
	"PSEM_RESPONSE_RNO",
	"PSEM_RESPONSE_ISSS",
	"PSEM_RESPONSE_FAIL"
};


// Table for lookup version of crc computation
//; CCIT table = x^16 + x^15 + x^2 + 1
UINT16          CCITShortTab[16] =
{
	0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xA50A, 0xB58B, 0xC60C, 0xD68D, 0xE70E, 0xF78F
};


//-------------------------------------


 // commport and current statistics
#ifdef __WIN32__
void *hComm ;

struct timeval tv0 ;

#endif

INT16           port;
INT16           baud;
char *user = NULL ;
char *pw = "00000000000000000000" ;  	// 20 zeros

int tables[128] ;
int ntables = 0 ;;

int verbose = 0 ;
int dosave = 0 ;
int doxml = 0 ;

char outdir[256] = "./" ;

// Set Read LP info helper. Called from gettab63() and gettab64() callback functions
void set_rlp_info(int rlpmode, readlp_info_t *pinfo)
{
	readlpmode = rlpmode ;

	if (pinfo) {
		memcpy((char *)&rlpinfo, pinfo, sizeof(readlp_info_t)) ;
		TRACE("==== off=%d\n", rlpinfo.lpset_offset) ;
	}
	else {
		memset((char *)&rlpinfo, 0, sizeof(readlp_info_t)) ;
	}
}

/*------------------------------------------------------------------------
************************   Protocol State Machines	**********************
------------------------------------------------------------------------*/

//------------------------------------------------------------------------
//	void SendMsg()
//	Kicks off send of message
//------------------------------------------------------------------------
//
void SendMsg()
{
	int rc ;

	if (nTxBytes > 0)	// must ensure > 0
	{
		// now we can transmit
		inTx = TRUE;

		rc = writeToSerialPort(hComm, pTxData, nTxBytes) ;
		if (rc > 0)
			nTxBytes -= rc ;

#ifdef __WIN32__
		if (nTxBytes == 0) {
			flushSerialPort(hComm) ;
			inTx = FALSE;
		}
#endif
	}
}

void SendAckNak(BYTE ack)
{
	txAckNak 	= ack;
	pTxData 	= &txAckNak;
	nTxBytes 	= 1;
	VVSHOWPACKET("Transmit ACK/NAK @", &txAckNak, 1);
	SendMsg();
}


//------------------------------------------------------------------------
//	void UpdateTxState()
//	updates the transmission of a message and/or ack/nak
//------------------------------------------------------------------------
//
void UpdateTxState()
{
	// finished transmitting a character
	if (inTx)
	{
		// must ensure > 0
		if (nTxBytes > 0)
		{
			// now we can transmit
			SendMsg() ;
			if (nTxBytes)
				inTx = FALSE;
		}
		else
			inTx = FALSE;

	}

	switch (txState)
	{
 		case TX_IDLE:
			if (haveTxRetry)
			{
				// initialize retry send of message
				haveTxRetry	= FALSE;
				txState 	= TX_MSG;
			}
			else if (haveTxMsg)
			{
				// initialize send of message
				haveTxMsg 	= FALSE;
				retryCount 	= MAX_RETRY;
				nLastTxMsg 	= nTxMsg;
				lastTxMsg 	= txBuffer;
				txState 	= TX_MSG;
			}
			break;
 
		case TX_MSG:
			// check if the last TX is still pending -- might happen if
			// an ACK/NAK prior to sending response
			if(inTx==TRUE)
				break;
					
			// start send of message
			nTxBytes 		= nLastTxMsg ;
			pTxData 		= lastTxMsg;

			SendMsg();
		
			waitForAckNak 	= TRUE;
			retryCount--;

			// go to wait for ack 
			txState = TX_WAITFORACK;
			SetN0MTimer(&tmrWaitForAck, WAITFORACK);

			VVSHOWPACKET("Transmit message  @", lastTxMsg, nLastTxMsg);
			if (verbose)
				parse_request(&lastTxMsg[6], nLastTxMsg-6-2) ;
			break;

		case TX_WAITFORACK:
			// check if the last TX is still pending
			if(inTx==TRUE)
				break;

			// Check if we received an ACK/NAK
			if (waitForAckNak == FALSE)
			{
				VVSHOWPACKET("Received ACK/NAK @", &rxAckNak, 1);
					
				// process depending on ACK or NAK
				if(rxAckNak == PSEM_ACK)
				{
					txState 		= TX_WAITFORRESPONSE;
					SetN0MTimer(&tmrRetry,WAITFORRESPONSE);
					haveResponse 	= FALSE;
					txFail			= FALSE;
				}
				else 
				{
					// if retryCount has tries left, resend or fail
					if (retryCount)
					{
						TRACE("Retrying transmit msg due to NAK: Try %d\n", MAX_RETRY - retryCount);
						txState = TX_IDLE;
						haveTxRetry	= TRUE;
					}
					else
						txState = TX_FAIL;
				}
			}
			else
			{
				// check for timeout
				if (CheckN0MTimer(&tmrWaitForAck))
				{
					if (retryCount)
					{
						TRACE("Retrying transmit msg due to timeout waiting for ACK : Try %d\n", MAX_RETRY - retryCount);
						txState = TX_IDLE;	// try again
						haveTxRetry	= TRUE;
					}
					else
						txState = TX_FAIL;	// go to idle state
				}
			}
			break;

		case TX_WAITFORRESPONSE:
			// Check for response wait timeout
			if(CheckN0MTimer(&tmrRetry))
			{
				// repsone didn't come in
				if(retryCount)
				{
					TRACE("Retrying transmit msg due to response timeout: Try %d\n", MAX_RETRY - retryCount);
					txState = TX_IDLE;	// retry send
					haveTxRetry	= TRUE;
				}
				else
					txState = TX_FAIL;
			}
			else
			{
				// check if response came in detected by receive state machine
				if(haveResponse == TRUE)
				{
					// Check if it was good response -- not duplicate and not bad
					if(txFail == TRUE)
					{
						if(txAckNak == PSEM_NAK)
						{
							// repsone didn't come in
							if(retryCount)
							{
								TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
								txState = TX_IDLE;	// retry send after sending NAK
							}
							else
								txState = TX_FAIL;
						}
						else
						{
							// it was duplicate, continue waiting
							haveResponse = FALSE; 
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
			haveResponse 	= TRUE;
			TRACE("TxMessage Failure @%d\n",ticCount);
			break;
			
		default:
			// catch fault -- should not get invalid state
			// Show error and reinitialize comm
			TRACE("Unknown TX service @%d\n",ticCount);
			InitializeComm(INITIALIZE_TX);
			break;
	}
}

//------------------------------------------------------------------------
//	void UpdateRxState()
//	updates the reception of a message and/or ack/nak
//------------------------------------------------------------------------
//
void UpdateRxState()
{
	UINT16          fcs;
	UINT16          i;
////////////////////
#ifdef __WIN32__
	unsigned char c ;
	int n ;
	
	gotChar = FALSE;

	n = readFromSerialPort(hComm, &c, 1) ;

	if (n == 1) {
		// check for receive of start of frame
		if (!inRx)
		{
			// framing looks for start of packet or ack/nak
			if (c == PSEM_STP)
			{
				inRx = TRUE;
			}
			else if (waitForAckNak)
			{
				// if waiting for acks see if we got it
				//  Note: skip over any garbage
				if( (c == PSEM_ACK) || (c == PSEM_NAK) )
				{
					rxAckNak = c;
					waitForAckNak = FALSE;
				}
			}

			nRxBytes = 0;
		}

		// if count of bytes received less than max
		if (nRxBytes < SIZERXBUF - 1)
		{
			// store it in receive buffer
			rxBuffer[nRxBytes++] = c;
			gotChar = TRUE;
		}
	}
#endif
////////////////////
	// update received character timer if necessary
	if (gotChar == TRUE)
	{
		gotChar = FALSE;
		
		// set timer to detect timeout for framing
		SetN0MTimer(&tmrSinceLast, INTER_CHARACTER_TIME_OUT);
	}
	
	// update application layer state message timeout if not in idle
	if((psemAppState != PSEM_APP_BASE) && (CheckN0MTimer(&tmrSession)) )
	{
		TRACE("Session timeout @%d\n",ticCount);
		psemAppState = PSEM_APP_BASE;
		rxToggle = PSEM_CTRL_TGL;
	}
	
	
	// update receive state machine for data link layer
	switch (rxState)
	{
		case RX_IDLE:
			// check for in receive of packet
			if (inRx == TRUE)
			{
				// we are change state
				rxState = RX_HEADER;
			}
			break;

		case RX_HEADER:
			// Check for message done
			if (nRxBytes >= sizeof(typePSEMHeader))
			{
				// compute length of message 
				nMsgBytes = sizeof(typePSEMHeader) - 1
					+ (rxBuffer[offsetof(typePSEMHeader, lengthmsb)] << 8)	// apdu
					+ rxBuffer[offsetof(typePSEMHeader, lengthmsb) + 1]
					+ 2;		// checksum

				// we are change state
				rxState = RX_BODY;
			}
			else if (CheckN0MTimer(&tmrSinceLast))
			{
				inRx 	= FALSE;
				rxState = RX_IDLE;
			}
			break;

		case RX_BODY:
			// Check for message done
			if (nRxBytes >= nMsgBytes)
			{
				// we have a message
				VVSHOWPACKET("Received message @", rxBuffer, nRxBytes);


				// check fcs
				CRCINITIALIZE(fcs);
				for (i = 0; i < nMsgBytes; i++)
					CRCUPDATECHAR(fcs, rxBuffer[i]);
				CRCFINALIZE(fcs);
				if (fcs == GOODFCS)
				{
					int cnt ;

					if (pcallback) {
						(pcallback)(&rxBuffer[6], nRxBytes-6-2) ;
						pcallback = NULL ;
					}

					cnt = parse_response(&rxBuffer[6], nRxBytes-6-2, (int) (((typePSEMHeader *)txBuffer)->service)) ;
					if (dosave != 0) {
						int svc ;
						int tableid ;

						svc = (int) (((typePSEMHeader *)txBuffer)->service) ;
						if ((svc == REQ_READ) || (svc == REQ_PREAD_OFF)) {
							tableid = txBuffer[7] ;
							tableid = (tableid<<8) + txBuffer[8] ;
							savetable(tableid, &rxBuffer[6+3], cnt /*nRxBytes-6-2*/) ;
						}
					}

					// good crc, ack it
					SendAckNak(PSEM_ACK);

					// process APDU
					// check for duplicate if same as last sequence number and same CRC
					if( (   rxToggle 	== (PSEM_CTRL_TGL & rxBuffer[offsetof(typePSEMHeader,ctrl)]) )
						&& (rxFCSHigh 	== rxBuffer[nMsgBytes-2])
						&& (rxFCSLow 	== rxBuffer[nMsgBytes-1])
						&& (rxID 		== rxBuffer[offsetof(typePSEMHeader,fill)]) )
					{
						// duplicate! we acked it, now back to idle
						TRACE("Received duplicate @%d\n",ticCount);
						inRx 			= FALSE;
						rxState 		= RX_IDLE;
 						break;
					}
					else
					{
						if(txState != TX_WAITFORRESPONSE)
						{
							TRACE("Received unexpected packet @%d\n",ticCount);
							inRx 			= FALSE;
							rxState 		= RX_IDLE;
	 						break;
						}
						else
						{
							// Its a good new packet, we can try to process at application layer
							// update for next time
							rxToggle 	= (BYTE)(PSEM_CTRL_TGL & rxBuffer[offsetof(typePSEMHeader,ctrl)]);
							rxFCSHigh 	= rxBuffer[nMsgBytes-2];
							rxFCSLow 	= rxBuffer[nMsgBytes-1];
							rxID		= rxBuffer[offsetof(typePSEMHeader,fill)];
							
							// process APDU
							rxState = RX_PROCESS;
						}
					}					
				}
				else
				{
					// its a bad packet or fragment -- NAK it
					TRACE("Received bad checksum @%d\n",ticCount);
					TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
					SendAckNak(PSEM_NAK);

					if(retryCount == 0)
					{
						haveResponse 	= TRUE;
						txFail			= TRUE;
					}

					// back to idle
					inRx 			= FALSE;
					rxState 		= RX_IDLE;
				}
			}
			else
			{
				// see if we waited too long
				if (CheckN0MTimer(&tmrSinceLast))
				{
					// timed out waiting for end of message
					TRACE("Timed out due to receive of fragment @%d\n",ticCount);
					TRACE("Sending NAK, and wait for response again : Try %d\n", MAX_RETRY - retryCount);
					SendAckNak(PSEM_NAK);

					if(retryCount == 0)
					{
						haveResponse 	= TRUE;
						txFail			= TRUE;
					}

					inRx 			= FALSE;
					rxState 		= RX_IDLE;
				}
			}
			break;

		case RX_PROCESS:
			// try to process message
			haveResponse = TRUE;
			if (DoPSEMClientApp())
			{
				// timed out waiting for end of message
				inRx 	= FALSE;		// free up receive buffer
				rxState = RX_IDLE;	// return to idle
			}
			break;

		default:
			// Show error and reinitialize comm
			TRACE("Unknown RX service @%d\n",ticCount);
			InitializeComm(INITIALIZE_RX);
			break;
	}
}

//------------------------------------------------------------------------
//	BOOL DoPSEMClientApp()
//	Processes an application layer message.  Message is to be handled, 
//	  providing response buffer is available. 
//		if buffer is not available, 
//	  		we will wait, return FALSE
// 		if successfully complete on message, return TRUE
//------------------------------------------------------------------------
//
BOOL DoPSEMClientApp()
{
	BYTE 			service;		// current app service invoked
	
	// initialize result code
	responseCode = rxBuffer[offsetof(typePSEMHeader, service)];
	service =  ((typePSEMHeader *)txBuffer)->service;
    
	// handle service request
	switch (service)
	{
		case PSEM_IDENT:
			VTRACE("PSEM Ident response: %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_ID;
			break;

		case PSEM_TERMINATE:
			VTRACE("PSEM Terminate response: %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_BASE;
			sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;
			break;			

			
		case PSEM_READ_OFFSET:
		case PSEM_READ:
			VTRACE("PSEM Read response: %s\n",responseNames[responseCode]);
			break;			


		case PSEM_WRITE:
		case PSEM_WRITE_OFFSET:
			VTRACE("PSEM Write response: %s\n",responseNames[responseCode]);
			break;

		case PSEM_LOGON:
			VTRACE("PSEM Logon response: %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_SESSION;
			break;
		
		case PSEM_SECURITY:
			VTRACE("PSEM Security response: %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_SESSION;
			break;

		case PSEM_LOGOFF:
			VTRACE("PSEM Logoff response: %s\n",responseNames[responseCode]);
			psemAppState =  PSEM_APP_ID;
			sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;
			break;
			
		case PSEM_NEGOTIATE:
			VTRACE("PSEM Negotiate response: %s\n",responseNames[responseCode]);
			break;
			
		case PSEM_WAIT:
			VTRACE("PSEM Wait response: %s\n",responseNames[responseCode]);
			// update our session timer value
			sessionTmrVal = (TIME_T)txBuffer[offsetof(typePSEMHeader, service)+1]*(TIME_T)10;
			break;
			
		default:
			// use simple response -- service not supported
			TRACE("Processed unkown response packet: %s\n",responseNames[responseCode]);
			break;
	}


    // We processed a message, should update Session timer
	SetN0MTimer(&tmrSession, sessionTmrVal);
	
	return (TRUE);
}



//------------------------------------------------------------------------
// UINT16                           // returns length of packet
// SetTxBuffer(PSEM_FRAME_FMT fmt, 	// the format of the frame 
// BYTE servicecode, 				// a service/response code
// BYTE * data, 				// data for the frame
// UINT16 length, 					// length of data
// PBYTE buffer, 					// buffer to place packet in
// UINT16 sizebuffer)				// size of destination buffer
//
//	SetTxBuffer is the packet assembly method for the protocol.  Based on 
// 	 what is passed to it, it writes a packet frame in transmit buffer to 
//	 send.
//------------------------------------------------------------------------
//
// returns length of packet
UINT16 SetTxBuffer(PSEM_FRAME_FMT fmt, 	// the format of the frame
BYTE servicecode, 					// a service / response code
BYTE * data, 					// data for the frame
UINT16 length, 						// length of data
PBYTE buffer, 						// buffer to place packet in
UINT16 sizebuffer)					// size of destination buffer
{
	UINT16          fcs;			// for frame check sequence
	UINT16          i;
	PBYTE           pT;
	BYTE            csum;
	UINT16          msgLength;		// the length of message + service

	// check if too large
	if (length >= sizebuffer)
	{
		TRACE("Error tried to overrun transmit buffer @%d\n",ticCount);
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
	*pT++ = (BYTE)(msgLength >> 8);		// length of message in bytes
	*pT++ = (BYTE)(msgLength & 0xff);	// length of message in bytes
	*pT++ = servicecode;		// PSEM service

	// check if this is request or PSEM response with data
	if (fmt == PSEM_FRAME_RESPONSE)
	{
		// its response with data = <count> <data> <cksum>
		*pT++ = (BYTE)(length >> 8);
		*pT++ = (BYTE)(length & 0xff);
		csum = 0;

		// copy in data and compute checksum
		for (i = 0; i < length; i++)
		{
			csum += data[i];
			*pT++ = data[i];
		}

		// place response data checksum in packet
		*pT++ = (BYTE)(~csum + 1);		// negate

	}
	else
	{
		// copy in data and compute checksum
		for (i = 0; i < length; i++)
			*pT++ = data[i];
	}
	
	// compute checksum
	msgLength = ((UINT16) (pT - buffer));
	pT = buffer;
	CRCINITIALIZE(fcs);
	for (i = 0; i < msgLength; i++)
	{
		CRCUPDATECHAR(fcs, *pT);
		pT++;
	}

	fcs = ~fcs;
	*pT++ = (BYTE)(fcs & 0xff);
	*pT++ = (BYTE)(fcs >> 8);

	// return total length of message
	return ((UINT16) (pT - buffer));
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
void InitializeComm(INITIALIZE_MYPLATFORM what)
{
	// Always initialize state of connection
	psemAppState = PSEM_APP_BASE;		// logical state of connection


	tmrSession = 0;				// time since last received character
	sessionTmrVal = CHANNEL_TRAFFIC_TIME_OUT;


	// initialize receive state machine
	if (what & INITIALIZE_RX)
	{
		nRxBytes 	= 0;
		inRx 		= FALSE;
		rxAckNak 	= 0;
		rxState 	= RX_IDLE;
		rxFCSLow 	= INITFCS & 0xff;
		rxFCSHigh 	= INITFCS >> 8;
		rxID		= 0xff;
		tmrSinceLast= 0;
		nMsgBytes 	= 0;
		rxToggle 	= PSEM_CTRL_TGL;
		gotChar		= FALSE;
		memset(rxBuffer, 0, SIZERXBUF);
	}

	// initialize transmit state machine
	if (what & INITIALIZE_TX)
	{
		nTxBytes 	= 0;
		pTxData 	= NULL;
		inTx 		= FALSE;
		txAckNak 	= 0;
		waitForAckNak = FALSE;
		txState 	= TX_IDLE;
		tmrWaitForAck = 0;
		haveTxMsg 	= FALSE;
		haveTxRetry = FALSE;
		nTxMsg 		= 0;
		txToggle 	= PSEM_CTRL_TGL;
		lastTxMsg 	= NULL;
		nLastTxMsg 	= 0;
		memset(txBuffer, 0, SIZETXBUF);
	}


	if (what & INITIALIZE_COMM)
	{
#ifdef __WIN32__
		char tpl[] = "\\\\.\\COM" ;
		char comport[16] ;

		sprintf(&comport[0], "%s%d", tpl, port) ;
		hComm = openSerialPort(comport, B9600, ONESTOPBIT, NOPARITY) ;
#endif

	}
}

//------------------------------------------------------------------------=
// void TerminateComm( )
//	Terminate -- probably only useful for PC simulation to restore ports
//------------------------------------------------------------------------=
//
void TerminateComm()
{
#ifdef __WIN32__
	closeSerialPort(hComm) ;
#endif

}


/*------------------------------------------------------------------------
**************************   Timer and timing support	******************
------------------------------------------------------------------------*/

//------------------------------------------------------------------------
//	void InitializePCTimer()
//	Sets up timer tic interrupt handler
//------------------------------------------------------------------------
//
void
InitializeN0MTimer()
{
#ifdef __WIN32__
	gettimeofday(&tv0, NULL) ;
#endif
	ticCount = 0;

}

//------------------------------------------------------------------------
//	void TerminatePCTimer()
//	Removes timer tic interrupt handler
//------------------------------------------------------------------------
//
void
TerminateN0MTimer()
{
}

//------------------------------------------------------------------------
//	void SetTimer()
//	Sets a timer, delt is specified in 1/10 of second
//------------------------------------------------------------------------
//
void SetN0MTimer(TIME_T * t, TIME_T delt)
{
#ifdef __WIN32__
	struct timeval tv ;
	unsigned long tt ;

	gettimeofday(&tv, NULL) ;
	tt = tv.tv_usec ;
	tt = (tt > tv0.tv_usec) ? tt - tv0.tv_usec : tv0.tv_usec - tt ;
	tt /= 1000 ; 	// make it milliseconds
	tt += (tv.tv_sec - tv0.tv_sec)*1000 ;
	tt /= MSEC_PER_TIC ;
	ticCount = tt ; // millisecond since program start
#endif

	if (delt > MAXTIMERVAL)
		*t = ticCount + MAXTIMERVAL - 1;
	else
		*t = ticCount + delt ;

	// make sure not 0 Note: will cause 1 tic error at 0
	if (*t == 0)
		(*t)++;
}

//------------------------------------------------------------------------
//	void CheckTimer()
//	Sets a timer
//------------------------------------------------------------------------
//
BOOL CheckN0MTimer(TIME_T * t)
{
#ifdef __WIN32__
	struct timeval tv ;
	unsigned long tt ;
#endif

	if (*t == 0)
		return (FALSE);			// timer is not active

#ifdef __WIN32__
	gettimeofday(&tv, NULL) ;
	tt = tv.tv_usec ;
	tt = (tt > tv0.tv_usec) ? tt - tv0.tv_usec : tv0.tv_usec - tt ;
	tt /= 1000 ; 	// make it milliseconds
	tt += (tv.tv_sec - tv0.tv_sec)*1000 ;
	tt /= MSEC_PER_TIC ;
	ticCount = tt ; // millisecond since program start
#endif

	// Timed out if ticCount has passed t by up to - MAXTIMERVAL
	if ((ticCount - *t) < (MAXTIMER - MAXTIMERVAL))
	{
		*t = 0;
		return (TRUE);
	}
	else
	{
		// else, we have time left
		return (FALSE);
	}
}

//------------------------------------------------------------------------
//	TIME_T GetTimer (TIME_T *t)
//	Gets a timer's remaining time
//------------------------------------------------------------------------
//
TIME_T GetN0MTimer(TIME_T * t)
{
#ifdef __WIN32__
	struct timeval tv ;
	unsigned long tt ;
#endif

	// define 0 as timer not active
	if (*t == 0)
		return (0);				// timer is not active

#ifdef __WIN32__
	gettimeofday(&tv, NULL) ;
	tt = tv.tv_usec ;
	tt = (tt > tv0.tv_usec) ? tt - tv0.tv_usec : tv0.tv_usec - tt ;
	tt /= 1000 ; 	// make it milliseconds
	tt += (tv.tv_sec - tv0.tv_sec)*1000 ;
	tt /= MSEC_PER_TIC ;
	ticCount = tt ; // ticks since program start
#endif

	// check if timer has elapsed
	if ((ticCount - *t) < (MAXTIMER - MAXTIMERVAL))
	{
		// if so, return 0
		return (0);
	}
	else
	{
		// otherwise return the time remaining
		return (*t - ticCount);
	}
}



/*------------------------------------------------------------------------
************************   	Main program		**************************
------------------------------------------------------------------------*/
void usage()
{
	TRACE(" Usage: \n");
	TRACE("  n0mcli [<flags>] [parameters]\n") ;
	TRACE("  flags :=\n") ;
	TRACE("    -x -- produce the .XML file named 'lpibYYYYMMDDHHNNSS.xml'\n") ;
	TRACE("    -s -- save the above tables in corresponding 'tablexxx.dat' files\n") ;
	TRACE("    -h -- help\n") ;
	TRACE("    -v -- verbose\n") ;
	TRACE("  parameters :=\n") ;
	TRACE("    n=<count> -- read <count> meter LP blocks. Defaolt 10. 0 means 'all available'.\n") ;
	TRACE("    d=<outdir> -- specifies the directory for output files. Current directory is used by default.\n") ;
	TRACE("    p=<n> -- use COMn for communication. Default is COM3\n") ;
	TRACE("    b=<baud> -- set specific COM port baudrate. Default is 9600\n") ;
	TRACE("    p=<n> -- use COMn for communication. Default is COM3\n") ;
	TRACE("    u=<string> -- set user id. Default is ''\n") ;
	TRACE("    pw=<string> -- set C12.19 password. Default is '00000000000000000000'\n") ;
	TRACE("    t=<list> -- read tables listed in comma-separated <list>, (tab0, tab61-64 always read)\n");
}

#ifdef __WIN32__
int kbhit()
{
	return 0;
}
#endif

void main(int argc, char ** argv)
{
	TIME_T  tmrSimulation;
	int messageCnt;
	BOOL done = FALSE;
	CLIENT_STATE clientState;
	int i, j, tmptab ;
	char *saveptr ;
	char *p ;


	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	//=================================
	// Check arguments -- 
	//=================================
	//
	// initialize default values
	port 	= 3;
	baud 	= 9600;
	
	for (i=1; i < argc; i++) {
		if (strncmp(argv[i], "-h", 2) == 0) {
			usage() ;
			return ;
		}
		if (strncmp(argv[i], "p=", 2) == 0) {
			port = atoi(&argv[i][2]) ;
			if (port < 1)
				port = 3 ;
		}
		else if (strncmp(argv[i], "b=", 2) == 0) {
			baud = atoi(&argv[i][2]) ;
			if ((baud < 300) || (baud > 115200))
				baud = 9600 ;
		}
		else if (strncmp(argv[i], "u=", 2) == 0) {
			user = &argv[i][2] ;
		}
		else if (strncmp(argv[i], "pw=", 3) == 0) {
			pw = &argv[i][3] ;
		}
		else if (strncmp(argv[i], "-vvv", 4) == 0) {
			verbose = 3 ;
		}
		else if (strncmp(argv[i], "-vv", 3) == 0) {
			verbose = 2 ;
		}
		else if (strncmp(argv[i], "-v", 2) == 0) {
			verbose = 1 ;
		}
		else if (strncmp(argv[i], "-s", 2) == 0) {
			dosave = 1 ;
		}
		else if (strncmp(argv[i], "-x", 2) == 0) {
			doxml = 1 ;
		}
		else if (strncmp(argv[i], "d=", 2) == 0) {
			int l ;

			strcpy(&outdir[0], &argv[i][2]) ;
			l = strlen(&outdir[0]) ;
			if (l) {
				if ((outdir[l-1] != '/') && (outdir[l-1] != '\\')) {
					outdir[l] = '/' ;
					outdir[l+1] = '\0' ;
				}
			}
		}
		else if (strncmp(argv[i], "n=", 2) == 0) {
			lp_blks_to_read = atoi(&argv[i][2]) ;
		}
		else if (strncmp(argv[i], "t=", 2) == 0) {
			p = strtok_r(&argv[i][2], ",", &saveptr) ;
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
		else {
			usage() ;
			return ;
		}
	}

	nScenarios = NUM_INTRO_TEST_MSGS + ntables + NUM_TRAILER_TEST_MSGS ;
	pScenarios = malloc(nScenarios * sizeof(typeScenario)) ;
	if (pScenarios == NULL) {
		TRACE("Can't allocate memory for %d scenarios\n") ;
		return ;
	}

	memcpy(pScenarios, &IntroScenarios, NUM_INTRO_TEST_MSGS * sizeof(typeScenario)) ;
	for (i=0; i < ntables; i++) {
		pScenarios[NUM_INTRO_TEST_MSGS+i].pData = pReadTabMsgs+i*sizeof(READTABx) ;
		pScenarios[NUM_INTRO_TEST_MSGS+i].length = sizeof(READTABx) ;
	}
	memcpy(pScenarios + NUM_INTRO_TEST_MSGS + ntables, &TrailerScenarios, NUM_TRAILER_TEST_MSGS * sizeof(typeScenario)) ;

	// initialize communications processes and timing interrupts
	InitializeComm(INITIALIZE_RX | INITIALIZE_TX | INITIALIZE_COMM);
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
	TRACE("***** Start of SE240 Client, Hit any key to exit ****\n");
	while (!kbhit() && !done)
	{
		// Update transmit state machine
		UpdateTxState();

		// Update receive state machine
		UpdateRxState();
		
		// process any waiting messages
		switch(clientState)
		{
		
			case CLIENT_IDLE:
				if(CheckN0MTimer(&tmrSimulation))
					clientState = CLIENT_REQUEST;
				break;
				
			case CLIENT_REQUEST:
				// Check for messages to send
				if(messageCnt < nScenarios /*NUM_TEST_MSGS*/)
				{
					// start new message
					if( SCENARIO_MSG(messageCnt)->service == 0xff )
					{
						// This is internal timer message
						SetN0MTimer(&tmrSimulation,
							( ((SCENARIO_MSG(messageCnt)->data[0])
							+ (SCENARIO_MSG(messageCnt)->data[1]<<8)) 
							* 10) );
						clientState = CLIENT_IDLE;
					}
					else
					{
						// This is PSEM message from script
						if (readlpmode) {
							unsigned int blkoffset ;
							unsigned char *p ;

							p = LPScenarios[0].pData ;

							blkoffset = rlpinfo.lpset_offset ;
							p[3] = (blkoffset >> 16) & 0xff ;
							p[4] = (blkoffset >> 8) & 0xff ;
							p[5] = blkoffset & 0xff ;
							p[6] = (rlpinfo.blksize >> 8) & 0xff ;
							p[7] = rlpinfo.blksize & 0xff ;

							nTxMsg = SetTxBuffer(
								PSEM_FRAME_SERVICE,
								(BYTE) (RLP_SCENARIO_MSG(0)->service),
								(BYTE *)(&RLP_SCENARIO_MSG(0)->data[0]),
								LPScenarios[0].length-1,
								txBuffer,
								SIZETXBUF);
							pcallback = LPScenarios[0].pcb ;
						}
						else {
							nTxMsg = SetTxBuffer(
									PSEM_FRAME_SERVICE,
									(BYTE) (SCENARIO_MSG(messageCnt)->service),
									(BYTE *)(&SCENARIO_MSG(messageCnt)->data[0]),
									pScenarios[messageCnt].length-1,
									txBuffer,
									SIZETXBUF);
							pcallback = pScenarios[messageCnt].pcb  ;
						}
						clientState 		= CLIENT_WAITFORRESPONSE;
						haveTxMsg 		= TRUE;
						haveResponse	= FALSE;
						if (pcallback) {
							VTRACE("\nNew CB request message @%d\n",ticCount);
						}
						else {
							VTRACE("\nNew request message @%d\n",ticCount);
						}
					}
					
					// stay on the same 'read LP block' message while in readLP mode
					if (readlpmode == 0)
						messageCnt++;
				}
				else
				{
					// Done simulation
					done = TRUE;
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

	if (doxml) {
		FILE *fp ;
		if ((fp=xmlopen(&outdir[0])) != NULL) {
			xmlgen(fp) ;
			xmlclose(fp) ;
		}
	}

	lputils_deinit() ;

	// clean up for PC exit
	TerminateN0MTimer();
	TerminateComm();

}
