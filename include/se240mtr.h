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

#ifndef _SE240METER_H_
#define _SE240METER_H_

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

//=========================================================================
// defines and typedefs
//=========================================================================
//
//
#define MSEC_PER_TIC 100


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
	uint8_t tableID;			// table number
	uint8_t location;			// storage location
	int canwrite;			// enabled to write
	uint16_t size;			// size of table
	uint8_t * tabledata;	// pointer to the data
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


int InitializeComm( INITIALIZE_MYPLATFORM what );
void TerminateComm(int fd);

uint16_t SetTxBuffer(PSEM_FRAME_FMT fmt, uint8_t servicecode, uint8_t * data, uint16_t length, uint8_t * buffer, uint16_t sizebuffer);
void SetN0MTimer (time_t *t, time_t delt);
int CheckN0MTimer (time_t *t);
time_t GetN0MTimer (time_t *t);


int UpdateTxState(int fd);
void UpdateRxState(int fd);
int DoPSEMApp();
uint8_t LookUpTableRcrd(uint16_t tableID);
int DoPSEMClientApp();

uint8_t DoRead(typeTableDef *pT, uint16_t offset, uint16_t count);
uint8_t DoWrite(typeTableDef *pT, uint16_t offset, uint16_t count, uint8_t * data);

#endif
