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


	se240mtr.h

	Author				: Dr. Martin J. Burns

	Date of Completion	: October 16, 1997
	Date last modified	: October 16, 1997

	Project				: EPRI SE-240 Meter Communications
	Tabs				: 4 for formatting printouts
	Language			: Ansi C
	Target				: MS-DOS

	Abstract:

	Copyright 1997 EPRI, ALL Rights Reserved


--------------------------------------------------------------------------
**********************	 Revision History.   *****************************
--------------------------------------------------------------------------


	10/16/97		0.0a	Original Version


*/



#ifndef __SE240METER_H
#define __SE240METER_H

//=========================================================================
// defines and typedefs
//=========================================================================
//
//
#ifdef __WIN32__
	#define MSEC_PER_TIC 100
#endif

#define MAXTIMER	( ((TIME_T)1)<<(TIMETBITS-1)-1) 
#define MAXTIMERVAL ((TIME_T)(MAXTIMER-MAXTIMER/10))

#define TABLE_NOT_FOUND 0xff

//=========================================================================
// Enumerations
//=========================================================================
//
//

typedef enum {
	INITIALIZE_RX 	= 1,	// Initialize Rx state machine
	INITIALIZE_TX 	= 2,	// Initialize Tx state machine
	INITIALIZE_COMM = 4		// Initialize Comm hardware
} INITIALIZE_MYPLATFORM;

typedef enum {
	TX_IDLE,				// transmit machine is in idle
	TX_MSG,					// transmit machine is txing MSG
	TX_WAITFORACK,			// waiting for an acknowledge
	TX_WAITFORRESPONSE,		// waiting for a response
	TX_FAIL,				// failed to succeed in transfer
} TX_STATE;

typedef enum {
	RX_IDLE,				// idle
	RX_HEADER,				// receiving a packet
	RX_BODY,				// receiving a packet
	RX_PROCESS,				// receiving a packet
} RX_STATE;

typedef enum {
	LOC_ROM,			// data is in ROM
	LOC_RAM,			// data is in RAM
	LOC_EEPROM			// data is in EEPROM
} DATALOCATION;			// where table data is stored
  
typedef struct {
	BYTE tableID;			// table number
	BYTE location;			// storage location
	BOOL canwrite;			// enabled to write
	UINT16 size;			// size of table
	BYTE FAR * tabledata;	// pointer to the data
} typeTableDef;  



//=============================================================================
//      MACROS
//=========================================================================
//

// CRC macros
#define CRCINITIALIZE(fcs) (fcs = INITFCS)
#define CRCUPDATECHAR(fcs,c)	\
		{fcs = (((fcs >> 4) & 0x0FFF) ^ CCITShortTab[((fcs ^ c) & 0x000F)]); \
		fcs = (((fcs >> 4) & 0x0FFF) ^ CCITShortTab[((fcs ^ (c>>4)) & 0x000F)]);} 
#define CRCFINALIZE(fcs) ( fcs = ~( (fcs >> 8) | (fcs << 8) ) )



//=========================================================================
// Prototypes
//=========================================================================
//
//


void InitializeComm( INITIALIZE_MYPLATFORM what );
void TerminateComm(void);

UINT16 SetTxBuffer(PSEM_FRAME_FMT fmt, BYTE servicecode, BYTE FAR * data, UINT16 length, PBYTE buffer, UINT16 sizebuffer);
void SetN0MTimer (TIME_T *t, TIME_T delt);
BOOL CheckN0MTimer (TIME_T *t);
TIME_T GetN0MTimer (TIME_T *t);


void UpdateTxState();
void UpdateRxState();
BOOL DoPSEMApp();
BYTE LookUpTableRcrd(UINT16 tableID);
BOOL DoPSEMClientApp();

BYTE DoRead(typeTableDef *pT, UINT16 offset, UINT16 count);
BYTE DoWrite(typeTableDef *pT, UINT16 offset, UINT16 count, PBYTE data);

#endif
