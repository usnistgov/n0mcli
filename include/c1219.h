/*
 * c1819.h
 *
 *  Created on: Mar 22, 2014
 *      Author: andy
 */

#ifndef _C1819_H_
#define _C1819_H_

#include <stdio.h>

typedef struct __attribute__((packed)) {
	unsigned char stp ;
	unsigned char rsv ;
	unsigned char ctl ;
	unsigned char seq ;
	unsigned char len_h ;
	unsigned char len_l ;
	unsigned char data[] ;
} c1218h_t ;

// read response header returned in case of SUCCESS
typedef struct __attribute__ ((packed)) {
	unsigned char okcode ;	// always zero
	unsigned char rspsize_h	;	// MSbyte of response data size
	unsigned char rspsize_l	;	// LSbyte of response data size
} readrsphdr_t ;

#define IS_SEGMENTED	0x80
#define IS_FIRST		0x40
#define TOGGLEBIT		0x20

#define REQ_IDENT 		0x20 // Identification request
#define REQ_TERMINATE 	0x21 // Terminate request

#define REQ_READ 		0x30 // Full Read request <tableid>
#define REQ_PREADI1 	0x31 // Partial Read 1 idx request <tableid> <index>+ <count>
#define REQ_PREADI2 	0x32 // Partial Read 2 idx request <tableid> <index>+ <count>
#define REQ_PREADI3 	0x33 // Partial Read 3 idx request <tableid> <index>+ <count>
#define REQ_PREADI4 	0x34 // Partial Read 4 idx request <tableid> <index>+ <count>
#define REQ_PREADI5 	0x35 // Partial Read 5 idx request <tableid> <index>+ <count>
#define REQ_PREADI6 	0x36 // Partial Read 6 idx request <tableid> <index>+ <count>
#define REQ_PREADI7 	0x37 // Partial Read 7 idx request <tableid> <index>+ <count>
#define REQ_PREADI8 	0x38 // Partial Read 8 idx request <tableid> <index>+ <count>
#define REQ_PREADI9 	0x39 // Partial Read 9 idx request <tableid> <index>+ <count>
#define REQ_PREAD_DFLT 	0x3E // Transfer Default Table
#define REQ_PREAD_OFF 	0x3F // Partial Read/Offset request <tableid> <offset> <count>
//		<tableid>		::=	<word16>	{ Table identifier }
//		<offset>		::=	<word24>	{ Offset into data table in bytes }
//		<index>			::=	<word16>	{ Index value used to locate start of data }
//		<count>			::=	<word16>	{ Length of table data requested, in bytes }

#define REQ_WRITE 		0x40 // Full Write request
#define REQ_PWRITEI1 	0x41 // Partial Write 1 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI2 	0x42 // Partial Write 2 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI3 	0x43 // Partial Write 3 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI4 	0x44 // Partial Write 4 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI5 	0x45 // Partial Write 5 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI6 	0x46 // Partial Write 6 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI7 	0x47 // Partial Write 7 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI8 	0x48 // Partial Write 8 idx request <tableid> <index>+ <count>
#define REQ_PWRITEI9 	0x49 // Partial Write 9 idx request <tableid> <index>+ <count>
#define REQ_PWRITE_OFF 	0x4F // Partial Write/Offset request <tableid> <offset> <count>

#define REQ_LOGON 		0x50 // Logon request
//		<user_id>	::=	<word16>	{User identification code}
//		<user>		::=	<byte>+10	{10 bytes containing user identification}

#define REQ_SECURITY 	0x51 // Security request
//		<password>	::= <byte>+20
#define REQ_LOGOFF 		0x52 // Logoff request

#define REQ_NEGOTIATE 	0x60 // Negotiate. No baudrate included/stay on default baudrate
#define REQ_NEGBR1 		0x61 // Negotiate/ 1 <baud rate> included in request
#define REQ_NEGBR2 		0x62 // Negotiate/ 2 <baud rate> included in request
#define REQ_NEGBR3 		0x63 // Negotiate/ 3 <baud rate> included in request
#define REQ_NEGBR4 		0x64 // Negotiate/ 4 <baud rate> included in request
#define REQ_NEGBR5 		0x65 // Negotiate/ 5 <baud rate> included in request
#define REQ_NEGBR6 		0x66 // Negotiate/ 6 <baud rate> included in request
#define REQ_NEGBR7 		0x67 // Negotiate/ 7 <baud rate> included in request
#define REQ_NEGBR8 		0x68 // Negotiate/ 8 <baud rate> included in request
#define REQ_NEGBR9 		0x69 // Negotiate/ 9 <baud rate> included in request
#define REQ_NEGBR10		0x6A // Negotiate/ 10 <baud rate> included in request
#define REQ_NEGBR11		0x6B // Negotiate/ 11 <baud rate> included in request
//		<packet_size>	::=	<word16> { Maximum packet size supported, in 
//			bytes. This value shall not be  greater than 8192 bytes.}
//		<nbr_packet>	::=	<byte>	{ Maximum number of packets this layer 
//			is able to reassemble into an upper  	layer data structure at one time. }
//		<baud_rate>		::=	00H |	{ Externally defined }
//						01H |	{ 300 baud }
//						02H |	{ 600 baud }
//						03H |	{ 1200 baud }
//						04H |	{ 2400 baud }
//						05H |	{ 4800 baud }
//						06H |	{ 9600 baud }
//						07H |	{ 14400 baud }
//						08H |	{ 19200 baud }
//						09H |	{ 28800 baud }	
//						0AH 	{ 56200 baud }
//						{ 	0BH - FFH reserved }

#define REQ_WAIT 		0x70 // Wait request
//		<wait>			::=	70H <time>	
//		<time>			::=	<byte>	{ Suggested wait period in seconds. }



////#define RSP_nok>	::=	<err>|<sns>|<isc>|<onp>|<iar>|<bsy>|<dnr>|<dlk>|<rno>|<isss>

#define RSP_OK		0x00 // Acknowledge - No problems, request accepted
#define RSPNOK_ERR	0x01 // Error - rejection of the received service reques
#define RSPNOK_SNS	0x02 // Service Not Supported
#define RSPNOK_ISC	0x03 // Insufficient Security Clearance
#define RSPNOK_ONP	0x04 // Operation Not Possible
#define RSPNOK_IAR	0x05 // Inappropriate Action Requested
#define RSPNOK_BSY	0x06 // Device Busy
#define RSPNOK_DNR	0x07 // Data Not Ready
#define RSPNOK_DLK	0x08 // Data Locked
#define RSPNOK_RNO	0x09 // Renegotiate request
#define RSPNOK_ISSS	0x0A // Invalid Service Sequence State

/***
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
						This byte shall be set to 00H until further defined.}

	Read response:
		<read_r>		::=	<nok> | <ok> <table_data>
		<table_data>	::=	<count> <data> <cksum>
		<count>		::=	<word16>	{ Length of <data> returned, in bytes }
		<data>			::=	<byte>+
		<cksum>		::=	<byte>	{ 2's compliment checksum computed only on 
				the <data> portion of <table_data>. 
				The checksum is computed by summing the bytes 
				(ignoring overflow) and negating the result}

	Write response:
		<write_r>		::=	<nok> | <ok>

	Logon Response:
		<logon_r>	::=	<err> | <bsy> | <iar> |	<isss> | <ok>

	Logoff Response:
		<logoff_r>		::=	<err> | <bsy> |	<isss> | <ok>

	Negotiate Response:
	<negotiate_r>	::=	<err> | <sns> |	<bsy> |	<isss> | <ok> <packet_size> <nbr_packet> 

	Wait Response:
		<wait_r>		::=	<err> |	<sns> |	<bsy> |	<isss> | <ok> 
	
***/

//////////////////////////////////////////////////////
//////// Table 0 - General Config
typedef struct __attribute__((packed)) {
	unsigned char data_order 	: 1 ;
	unsigned char char_format 	: 3 ;
	unsigned char model_select	: 3 ;
	unsigned char filler		: 1 ;
} FORMAT_CONTROL_1_BFLD ;

typedef struct __attribute__((packed)) {
	unsigned char tm_format 	: 3 ;
	unsigned char data_access_method 	: 2 ;
	unsigned char id_form		: 1 ;
	unsigned char int_format	: 2 ;
} FORMAT_CONTROL_2_BFLD ;

typedef struct __attribute__((packed)) {
	unsigned char ni_format1 	: 4 ;
	unsigned char ni_format2	: 4 ;
} FORMAT_CONTROL_3_BFLD ;

typedef struct __attribute__((packed)) {
	FORMAT_CONTROL_1_BFLD	format_control_1 ;
	FORMAT_CONTROL_2_BFLD	format_control_2 ;
	FORMAT_CONTROL_3_BFLD	format_control_3 ;
	unsigned char device_class[4] ;			/* This Element contains a relative universal object identifier
												derived from the following structure:
												<device-class-root-oid>.<class>
												The <class> (DEVICE_CLASS) is registered in a manner that it 
												shall be unique among all C12.19 implementations, within a 
												registered root context. This numeric sequence is encoded
												according to ISO 8825-1-1997 "Basic Encoding Rules". */
	unsigned char nameplate_type ;			/* Entry used to select the nameplate record
												structure to be used in DEVICE_NAMEPLATE_TBL (Table 02) */
	unsigned char default_set_used ;		/* Indicates which, if any, default sets are used */
	unsigned char max_proc_parm_length ;	/* Manufacturer-defined maximum length for argument parameters 
												passed to procedures in PROC_INITIATE_TBL (Table 07) */
	unsigned char max_resp_data_len ;		/* Manufacturer-defined maximum length representing length returned 
												by procedures in PROC_RESPONSE_TBL (Table 08). */
	unsigned char std_version_no ;			/* A number that designates the version of the particular industry 
												set of Standard Tables */
	unsigned char std_revision_no ;			/* A number that designates a minor change to a version of this 
												standard. Within a given version of this standard, all revisions 
												with a lower revision number shall be backward compatible. If 
												this is not true then a new version number shall be required */
	unsigned char dim_std_tbls_used ;		/* The number of octets required to represent the set of standard 
												tables used in GEN_CONFIG_TBL.STD_TBLS_USED (Table 00) */
	unsigned char dim_mfg_tbls_used ;		/* The number of octets required to represent the set of manufacturer 
												tables used in GEN_CONFIG_TBL.MFG_TBLS_USED (Table 00). */
	unsigned char dim_std_proc_used ;		/* Octets required to represent the set of standard procedures used 
												in GEN_CONFIG_TBL.STD_PROC_USED (Table 00) */
	unsigned char dim_mfg_proc_used ;		/* The number of octets required to represent the set of manufacturer 
												procedures used in GEN_CONFIG_TBL.MFG_PROC_USED (Table 00) */
	unsigned char dim_mfg_status_used ;		/* The number of octets allocated for indicating manufacturer specific 
												status flags */
	unsigned char nbr_pending ;				/* The number of pending status sets in PENDING_STATUS_TBL (Table 04). */
/*** Variable Part ...
	std_tbls_used : set(gen_config_tbl.dim_std_tbls_used);
	mfg_tbls_used : set(gen_config_tbl.dim_mfg_tbls_used);
	std_proc_used : set(gen_config_tbl.dim_std_proc_used);
	mfg_proc_used : set(gen_config_tbl.dim_mfg_proc_used);
	std_tbls_write : set(gen_config_tbl.dim_std_tbls_used);
	mfg_tbls_write : set(gen_config_tbl.dim_mfg_tbls_used);
***/
} GEN_CONFIG_RCD ;

#define DATA_ORDER_LSBFIRST	0
#define DATA_ORDER_MSBFIRST	1

//#define	CHAR_FORMAT // see STD.CHAR_FORMAT_ENUM
#define CHAR_FORMAT_ISO7BIT	1
#define CHAR_FORMAT_ISO8BIT	2
#define CHAR_FORMAT_UTF8	3

#define MODEL_SELECT_TBL16	0

//#define TM_FORMAT	// see STD.TM_FORMAT_ENUM
#define TM_FORMAT_NO 0 		//No clock in the End Device.
#define TM_FORMAT_BCD 1		// BCDtype with discrete fields for year, month,
							// day, hour, minute seconds and fractional seconds.
#define TM_FORMAT_UINT8 2	// UINT8 type with discrete fields for year, month,
							// day, hour, minute seconds and fractional seconds.
#define TM_FORMAT_UINT32 3	// UINT32 counters where HTIME_DATE, LTIME_DATE and 
							// STIME_DATE types are encoded relative to 01/01/1970 @ 00:00:00
							// UTC, with discrete fields for minutes and fraction of a minute.
#define TM_FORMAT_UINT32S 4	// UINT32 counters where HTIME_DATE, LTIME_DATE and 
							// STIME_DATE types are encoded relative to 01/01/1970 @ 00:00:00
							// UTC), with discrete fields for seconds and fraction of a second.

#define DATA_ACCESS_METHOD_FULLONLY	0
#define DATA_ACCESS_METHOD_FULLOFF	1
#define DATA_ACCESS_METHOD_FULLIDX	2
#define DATA_ACCESS_METHOD_ANY	3

#define ID_FORM_STRING	0
#define ID_FORM_BCD		1

#define INT_FORMAT_TWOSCOMP	0
#define INT_FORMAT_ONESCOMP	1
#define INT_FORMAT_SIGMAG	2

//#define NI_FORMAT1	// see STD.NI_FORMAT_ENUM
//#define NI_FORMAT2	// see STD.NI_FORMAT_ENUM
/*
 Values that may be assumed by  GEN_CONFIG_TBL.FORMAT_CONTROL_3.NI_FORMAT1
 (which controls the interpretation of the built-in type NI_FMAT1) and
 GEN_CONFIG_TBL.FORMAT_CONTROL_3.NI_FORMAT2 (which controls the interpretation 
 of the built-in type NI_FMAT2).*/
typedef enum {
	NI_FORMAT_FLOAT64 = 0,
	NI_FORMAT_FLOAT32 = 1,
	NI_FORMAT_FLOAT_CHAR12 = 2, // (A STRING Number)
	NI_FORMAT_FLOAT_CHAR6 = 3,  // (A STRING Number)
	NI_FORMAT_INT32_1 = 4,	// (Implied decimal point between fourth and fifth digits from
							// least significant digit. For example 0.0001 is represented as 1)
	NI_FORMAT_FIXED_BCD6 = 5,
	NI_FORMAT_FIXED_BCD4 = 6,
	NI_FORMAT_INT24 = 7,
	NI_FORMAT_INT32 = 8,
	NI_FORMAT_INT40 = 9,
	NI_FORMAT_INT48 = 10,
	NI_FORMAT_INT64 = 11,
	NI_FORMAT_FIXED_BCD8 = 12,
	NI_FORMAT_FLOAT_CHAR21 = 13 // (A STRING Number)
} NI_FMT ;

#define NAMEPLATE_TYPE_GAS	0
#define NAMEPLATE_TYPE_WATER	1
#define NAMEPLATE_TYPE_ELECTRIC	2

#define DEFAULT_SET_NONE	0	// Default sets are not used
#define DEFAULT_SET_1		1	// Default set #1, Simple Meter Register, in use
#define DEFAULT_SET_2		2	// Default set #2, Simple Demand Meter, in use
#define DEFAULT_SET_3		3	// Default set #3, Simple TOU Meter, in use 
#define DEFAULT_SET_4		4	// Default set #4, Simple Profile Recorder, in use

//////// Table 01 - General Manufacturer Identification Table
typedef struct  __attribute__((packed)) {
	unsigned char manufacturer[4] ;			// Acronym for the manufacturer name.
	unsigned char ed_model[8] ; 			// Model identifier of the End Device left justified.
	unsigned char hw_version_number ;		// 0..255 Manufacturer's hardware version number.
	unsigned char hw_revision_number ; 		// 0..255 Manufacturer's hardware revision number.
	unsigned char fw_version_number ;		// 0..255 Manufacturer's firmware version number.
	unsigned char fw_revision_number ;		// 0..255 Manufacturer's firmware revision number.
	unsigned char mfg_serial_number[16] ;	// Manufacturer's serial number. May be a BCD[8] number, depemding on GEN_CONFIG_TBL.ID_FORM
} MANUFACTURER_IDENT_RCD ;

//////// Table 03 - End Device Mode Status Table
typedef struct  __attribute__((packed)) {
	unsigned char metering_flag : 1 ;			// TRUE End Device is in Metering Mode.
	unsigned char test_mode_flag : 1 ;			// TRUE End Device is in Test Mode.
	unsigned char meter_shop_mode_flag : 1 ;	// TRUE End Device is in Meter Shop Mode.
	unsigned char factory_flag : 1 ;			// TRUE End Device is in Factory Mode.
	unsigned char filler : 4 ;
} ED_MODE_BFLD ;
typedef struct  __attribute__((packed)) {
	unsigned short unprogrammed_flag : 1 ; 			// TRUE End Device is not programmed or it is in a factory default state.
	unsigned short configuration_error_flag : 1 ; 	// TRUE End Device did detect a configuration error.
	unsigned short self_chk_error_flag : 1 ; 		// TRUE End Device did detect a self check error.
	unsigned short ram_failure_flag : 1 ; 			// TRUE End Device did detect a RAM Memory failure.
	unsigned short rom_failure_flag : 1 ; 			// TRUE End Device did detect a ROM Memory failure.
	unsigned short nonvol_mem_failure_flag : 1 ; 	// TRUE End Device did detect a nonvolatile memory failure.
	unsigned short clock_error_flag : 1 ; 			// TRUE End Device did detect a clock error.
	unsigned short measurement_error_flag : 1 ; 	// TRUE End Device did detect a measurement element error.
	unsigned short low_battery_flag : 1 ; 			// TRUE End Device did detect a low battery error.
	unsigned short low_loss_potential_flag : 1 ; 	// TRUE End Device did detect a device potential that is below a predetermined value.
	unsigned short demand_overload_flag : 1 ; 		// TRUE End Device did detect a demand threshold overload.
	unsigned short power_failure_flag : 1 ; 		// TRUE End Device did detect a power failure.
	unsigned short tamper_detect_flag : 1 ; 		// TRUE End Device did detect tamper activity.
	unsigned short reverse_rotation_flag : 1 ; 		// TRUE End Device did detect reverse rotation.
	unsigned short filler : 2 ; 					// (14..15);
} ED_STD_STATUS1_BFLD ;
typedef struct  __attribute__((packed)) {
	unsigned char filler ; // Standard status code bit field 2 is a place holder for future expansion.
} ED_STD_STATUS2_BFLD ;
typedef struct  __attribute__((packed)) {
	unsigned char ed_mfg_status ; // Set containing the manufacturer specific status flags.
} ED_MFG_STATUS_RCD ;

typedef struct  __attribute__((packed)) {
	ED_MODE_BFLD ed_mode  ;
	ED_STD_STATUS1_BFLD ed_std_status1  ;
	ED_STD_STATUS2_BFLD ed_std_status2  ;
	ED_MFG_STATUS_RCD ed_mfg_status  ;
} ED_MODE_STATUS_RCD ;

//////// Table 11 - Actual Data Sources Limiting Table
typedef struct  __attribute__((packed)) {
	unsigned char pf_exclude_flag : 1 ; // TRUE Power fail exclusion is active.
	unsigned char reset_exclude_flag : 1 ; // TRUE Reset exclusion is in use by the End Device.
	unsigned char block_demand_flag : 1 ; // TRUE Block demand is in use by the End Device.
	unsigned char sliding_demand_flag : 1 ; // TRUE Sliding demand is in use by the End Device.
	unsigned char thermal_demand_flag : 1 ; // TRUE Thermal demand is in use by the End Device.
	unsigned char set1_present_flag : 1 ; // TRUE The End Device is using the first set of optional constants in the electric record of the CONSTANTS_TBL (Table 15).
	unsigned char set2_present_flag : 1 ; // TRUE The End Device is using the second set of	optional constants in the electric record of the CONSTANTS_TBL (Table 15).
	unsigned char conversion_alg_flag : 1 ; // conversion of measurements to engineering values algo(see Annex K) is implemented by this End Device.
} SOURCE_FLAGS_BFLD ;

typedef struct  __attribute__((packed)) {
	SOURCE_FLAGS_BFLD source_flags  ;	// what demand and TOU regs functions are in use by the End Device.
	unsigned char nbr_uom_entries ;		// Actual number of entries in UOM_ENTRY_TBL (Table 12).
	unsigned char nbr_demand_ctrl_entries ; // Actual number of entries in the DEMAND_CONTROL_TBL (Table 13).
	unsigned char data_ctrl_length ;	// value that determines the width in octets of an entry in the first array of the DATA_CONTROL_TBL (Table 14).
	unsigned char nbr_data_ctrl_entries ; // Actual number of entries in the DATA_CONTROL_TBL (Table 14).
	unsigned char nbr_constants_entries ; // Actual number of entries in the CONSTANTS_TBL (Table 15).
	unsigned char constants_selector ; //   data structure used in the CONSTANTS_TBL (Table 15): 0 - GAS_CONSTANTS_AGA3_RCD, 1 - GAS_CONSTANTS_AGA7_RCD 2 - ELECTRIC_CONSTANTS_RCD
	unsigned char nbr_sources ;	// Actual number of entries in the SOURCES_TBL (Table 16).
} SOURCE_RCD ;

//////// Table 12 - Units of Measure Entry Table
/***
UOM_ENTRY_TBL (Table 12) provides a method for describing data attributes. It may be used to tag
data sources. One entry exists in this Table (UOM_ENTRY_TBL) only for each entry in
SOURCES_TBL.SOURCE_LINK_RCD.UOM_ENTRY_FLAG that is set to TRUE. Therefore the
application has to perform a “bit” walk across all UOM_ENTRY_FLAG in SOURCES_TBL that are set to
TRUE, in order to locate the related SELECTION array Element found in this Table (UOM_ENTRY_TBL).
***/

typedef struct  __attribute__((packed)) {
	unsigned int id_code : 8 ; // 0 Active power – W. 1 Reactive power – VAR. 2 Apparent power – VA etc.
	unsigned int time_base : 3 ; // uint(8..10);
	unsigned int multiplier : 3 ; // identifies the decimal scaling value to apply to the reported value after delivery to the application of all conversion constants of the tagged item
	unsigned int q1_accountability : 1 ; // Tagged item is in quadrant 1. It shall be included accounting or measurement.
	unsigned int q2_accountability : 1 ; // Tagged item is in quadrant 2. It shall be included accounting or measurement.
	unsigned int q3_accountability : 1 ; // Tagged item is in quadrant 3. It shall be included accounting or measurement.
	unsigned int q4_accountability : 1 ; // Tagged item is in quadrant 4. It shall be included accounting or measurement.
	unsigned int net_flow_accountability : 1 ; // Net of delivered - received ...
	unsigned int segmentation : 3 ; // uint(19..21);
	unsigned int harmonic : 1 ; // The identified ID_CODE is a harmonic component of an associated source.
	unsigned int id_resource : 5 ; //uint(23..27);
	unsigned int reserved : 3 ; //fill(28..30);
	unsigned int nfs : 1 ; //This unit of measure entry description does not follow the unit of measure definitions defined in this standard.
} UOM_ENTRY_BFLD ;
/***
TYPE UOM_ENTRY_RCD = PACKED RECORD
UOM_ENTRY : ARRAY[ACT_SOURCES_LIM_TBL.NBR_UOM_ENTRIES] OF
UOM_ENTRY_BFLD;
END;
TABLE 12 UOM_ENTRY_TBL = UOM_ENTRY_RCD;
***/

//////// Table 15 - Constants Table
/***
 * we currently don't use Table 15 ...
 */

//////// Table 16 - Source Definition Table
typedef struct  __attribute__((packed)) {
	unsigned char uom_entry_flag : 1 ;	// A UOM_ENTRY_TBL (Table 12) entry is associated with this source.
	unsigned char demand_ctrl_flag : 1 ;// A DEMAND_CONTROL_TBL (Table 13) entry is associated with this source.
	unsigned char data_ctrl_flag : 1 ;	// A DATA_CONTROL_TBL (Table 14) entry is associated with this source.
	unsigned char constants_flag : 1 ;	// A CONSTANTS_TBL (Table 15) entry is associated with this source.
	unsigned char pulse_engr_flag : 1 ;	// The source is in engineering units.(in pulse units otherwise)
	unsigned char constant_to_be_applied : 1 ; // The entry in the CONSTANTS_TBL (Table 15) if present, have to be applied
	unsigned char filler: 2 ;
} SOURCE_LINK_BFLD ;

/***
TYPE SOURCE_LINK_RCD = PACKED RECORD
SOURCES_LINK : ARRAY[ACT_SOURCES_LIM_TBL.NBR_SOURCES] OF
SOURCE_LINK_BFLD;
END;
TABLE 16 SOURCES_TBL = SOURCE_LINK_RCD;
***/

//////// Table 21 - Actual Register Limiting Table
typedef struct  __attribute__((packed)) {
	unsigned char season_info_field_flag : 1 ;	// Device is reporting the representative season in Tables in this Decade.
	unsigned char date_time_field_flag : 1 ;	// Device is providing the date and time in Tables in this Decade.
	unsigned char demand_reset_ctr_flag : 1 ;	// Device is counting the number of activations of Demand resets.
	unsigned char demand_reset_lock_flag : 1 ;	// Demand reset lockout is enabled.
	unsigned char cum_demand_flag : 1 ;			// Cumulative Demand is in use by the Device.
	unsigned char cont_cum_demand_flag : 1 ;	// Continuous Cumulative Demand is in use by the Device.
	unsigned char time_remaining_flag : 1 ;		// Device reports the time remaining in Demand interval.
	unsigned char filler : 1 ;
} REG_FUNC1_BFLD ;
typedef struct  __attribute__((packed)) {
	unsigned char self_read_inhibit_overflow_flag : 1 ;	// Device inhibits Self-reads once an overflow occurs.
	unsigned char self_read_seq_nbr_flag : 1 ;	// Device is providing a Self-read sequential number.
	unsigned char daily_self_read_flag : 1 ;	// Daily Self-reads are in use.
	unsigned char weekly_self_read_flag : 1 ;	// Weekly Self-reads are in use.
	unsigned char self_read_demand_reset : 2 ;	// Specifies whether the End Device will perform a Self-read on a Demand reset
	unsigned char filler : 2 ;
} REG_FUNC2_BFLD ;

typedef struct  __attribute__((packed)) {
	REG_FUNC1_BFLD reg_func1_flags ;
	REG_FUNC2_BFLD reg_func2_flags ;
	unsigned char nbr_self_reads ;			// Number of Self-reads in use.
	unsigned char nbr_summations ;			// Number of summation Registers in each data block
	unsigned char nbr_demands ;				// Number of Demand Registers in each data block.
	unsigned char nbr_coin_values ;			// Number of coincident values saved in each data block.
	unsigned char nbr_occur ;				// Number of occurrences stored for a particular selection.
	unsigned char nbr_tiers ;				// Number of tiers in use.
	unsigned char nbr_present_demands ;		// Number of present demand values that are stored.
	unsigned char nbr_present_values ;		// Number of present values that are stored.
} REGS_RCD ;

//////// Table 22 - Data Selection Table
/***
DATA_SELECTION_TBL (Table 22) contains grouped lists of source indices. These indices point
towards Array elements in SOURCES_TBL (Table 16) when Table 22 is present. These lists are used to
build the Table that contains the data to be captured. The groupings are described below.
Global Default Table Property Overrides: Role=“CONTROL”
Table 22 Type Definitions
TYPE DATA_SELECTION_RCD = PACKED RECORD
	SUMMATION_SELECT : ARRAY[ACT_REGS_TBL.NBR_SUMMATIONS] OF STD.SOURCE_SELECT_RCD;
	DEMAND_SELECT : ARRAY[ACT_REGS_TBL.NBR_DEMANDS] OF STD.SOURCE_SELECT_RCD;
	MIN_OR_MAX_FLAGS : SET((ACT_REGS_TBL.NBR_DEMANDS+7)/8);
	COINCIDENT_SELECT : ARRAY[ACT_REGS_TBL.NBR_COIN_VALUES] OF STD.SOURCE_SELECT_RCD;
	COIN_DEMAND_ASSOC : ARRAY[ACT_REGS_TBL.NBR_COIN_VALUES] OF UINT8;
END;

SUMMATION_SELECT
  List of source selectors that groups together
  bulk/energy/summation that are associated with
  the SUMMATIONS defined in this decade.
DEMAND_SELECT
  List of source selectors that groups together
  demand sources that are associated with
  DEMANDS defined in this decade.
MIN_OR_MAX_FLAGS
  SET of bit flags, each corresponding to an entry
  in DEMAND_SELECT.
  FALSE Indicates that the associated
    DEMAND_SELECT entry is a minimum.
  TRUE Indicates that the associated
    DEMAND_SELECT entry is a maximum.
COINCIDENT_SELECT
  Data source selectors for quantites to be placed
  in the COINCIDENTS array of the Current
  Register Data Table (Table 23).
COIN_DEMAND_ASSOC
  Each entry corresponds to an entry in
  COINCIDENT_SELECT and provides an index
  into DEMAND_SELECT identifying the Demand
  for which this coincident value is taken.

***/

//////// Table 23 - Current Register Data Table
/***
contains the current Register data. “Current” is with respect to
the previous demand reset, season or Self-read. Table 23 is intended for billing purposes.

Table 23 Type Definitions
TYPE COINCIDENTS_RCD = PACKED RECORD
	COINCIDENT_VALUES : ARRAY[ACT_REGS_TBL.NBR_OCCUR] OF NI_FMAT2;
END;
TYPE DEMANDS_RCD = PACKED RECORD
	IF ACT_REGS_TBL.DATE_TIME_FIELD_FLAG THEN
		EVENT_TIME : ARRAY[ACT_REGS_TBL.NBR_OCCUR] OF STIME_DATE;
	END;
	IF ACT_REGS_TBL.CUM_DEMAND_FLAG THEN
		CUM_DEMAND : NI_FMAT1;
	END;
	IF ACT_REGS_TBL.CONT_CUM_DEMAND_FLAG THEN
		CONT_CUM_DEMAND : NI_FMAT1;
	END;
	DEMAND : ARRAY[ACT_REGS_TBL.NBR_OCCUR] OF NI_FMAT2;
END;

TYPE DATA_BLK_RCD = PACKED RECORD
	SUMMATIONS : ARRAY[ACT_REGS_TBL.NBR_SUMMATIONS] OF NI_FMAT1;
	DEMANDS : ARRAY[ACT_REGS_TBL.NBR_DEMANDS] OF DEMANDS_RCD;
	COINCIDENTS : ARRAY[ACT_REGS_TBL.NBR_COIN_VALUES] OF COINCIDENTS_RCD;
END;

TYPE REGISTER_DATA_RCD = PACKED RECORD
	IF ACT_REGS_TBL.DEMAND_RESET_CTR_FLAG THEN
		NBR_DEMAND_RESETS : UINT8;
	END;
	TOT_DATA_BLOCK : DATA_BLK_RCD;
	TIER_DATA_BLOCK : ARRAY[ACT_REGS_TBL.NBR_TIERS] OF DATA_BLK_RCD;
END;

TABLE 23 CURRENT_REG_DATA_TBL = REGISTER_DATA_RCD;
***/

//////// Table 27 - Present Register Selection Table
/***
provides selections to sources of present demands
and present values. These indices point towards array elements in SOURCES_TBL (Table 16) when
Table 16 is present. The values selected are available in PRESENT_REGISTER_DATA_TBL (Table
28). Table 27 is used to select sources for Table 28, which is continually updated.

Table 27 Type Definitions
TYPE PRESENT_REGISTER_SELECT_RCD = PACKED RECORD
	PRESENT_DEMAND_SELECT : ARRAY[ACT_REGS_TBL.NBR_PRESENT_DEMANDS] OF STD.SOURCE_SELECT_RCD;
	PRESENT_VALUE_SELECT : ARRAY[ACT_REGS_TBL.NBR_PRESENT_VALUES] OF
	STD.SOURCE_SELECT_RCD;
END;

TABLE 27 PRESENT_REGISTER_SELECT_TBL = PRESENT_REGISTER_SELECT_RCD;
***/

//////// Table 28 - Present Register Data Table
/***
contains the present demand and values as selected by
the PRESENT_REGISTER_SELECT_TBL (Table 27). Table 28 is continually updated.

Table 28 Type Definitions
TYPE PRESENT_DEMAND_RCD = PACKED RECORD
	IF ACT_REGS_TBL.TIME_REMAINING_FLAG THEN
		TIME_REMAINING : TIME;
	END;
	DEMAND_VALUE : NI_FMAT2;
END;

TYPE PRESENT_REGISTER_DATA_RCD = PACKED RECORD
	PRESENT_DEMAND : ARRAY[ACT_REGS_TBL.NBR_PRESENT_DEMANDS] OF PRESENT_DEMAND_RCD;
	PRESENT_VALUE : ARRAY[ACT_REGS_TBL.NBR_PRESENT_VALUES] OF NI_FMAT1;
END;

TABLE 28 PRESENT_REGISTER_DATA_TBL = PRESENT_REGISTER_DATA_RCD;
***/



//////////////////// Table 52 - Clock

typedef struct __attribute__((packed)) {
	unsigned char nibble1:4 ;
	unsigned char nibble0:4 ;
} BCD ;

typedef union __attribute__((packed)) {
	struct __attribute__((packed)) {
		void * nil ;
	} case0 ;
	struct __attribute__((packed)) {
		BCD	year ;
		BCD month ;
		BCD day ;
		BCD hour ;
		BCD minute ;
		BCD second ;
	} case1 ;
	struct __attribute__((packed)) {
		 unsigned char	year ;
		 unsigned char month ;
		 unsigned char day ;
		 unsigned char hour ;
		 unsigned char minute ;
		 unsigned char second ;
	} case2 ;
	struct __attribute__((packed)) {
		unsigned int u_time ;
		unsigned char second ;
	} case3 ;
	struct __attribute__((packed)) {
		unsigned int u_time_sec ;
	} case4 ;
} LTIME_DATE ;

typedef union __attribute__((packed)) {
	struct {
		void * nil ;
	} case0 ;
	struct __attribute__((packed)) {
		BCD	year ;
		BCD month ;
		BCD day ;
		BCD hour ;
		BCD minute ;
	} case1 ;
	struct __attribute__((packed)) {
		 unsigned char	year ;
		 unsigned char month ;
		 unsigned char day ;
		 unsigned char hour ;
		 unsigned char minute ;
	} case2 ;
	struct __attribute__((packed)) {
		unsigned int u_time ;
	} case3 ;
	struct __attribute__((packed)) {
		unsigned int u_time_sec ;
	} case4 ;
} STIME_DATE ;


typedef struct __attribute__((packed)) {
	unsigned char day_of_week : 3 ;
	unsigned char dst_flag : 1 ;
	unsigned char gmt_flag : 1 ;
	unsigned char tm_zn_applied_flag : 1 ;
	unsigned char dst_applied_flag : 1 ;
	unsigned char dst_supported_flag : 1 ;
} TIME_DATE_QUAL_BFLD ;

typedef struct __attribute((packed)) {
	LTIME_DATE clock_calendar ;
	TIME_DATE_QUAL_BFLD time_date_qual ;
} CLOCK_STATE_RCD ;

#define DOW_SUN 0  // Sunday.
#define DOW_MON 1 // Monday.
#define DOW_TUE 2 // Tuesday.
#define DOW_WED 3 // Wednesday.
#define DOW_THU 4 // Thursday.
#define DOW_FRI 5 // Friday.
#define DOW_SAT 6 // Saturday.
#define DOW_UND 7 // DAY_OF_WEEK is undefined, this field is not supported by the End Device.

////////////// Table 60 Load Profile Dimension Limits
typedef struct __attribute__((packed)) {
	// if set -- the End Device is capable of inhibiting Load Profile set #N once an overflow occurs 
	unsigned short lp_set1_inhibit_ovf_flag : 1 ;	
	unsigned short lp_set2_inhibit_ovf_flag : 1 ;
	unsigned short lp_set3_inhibit_ovf_flag : 1 ;
	unsigned short lp_set4_inhibit_ovf_flag : 1 ;
	// if set -- the End Device is capable of providing blockend Register reading information
	unsigned short blk_end_read_flag : 1 ;
	// if set -- the End Device is capable of providing blockend pulse accumulator data
	unsigned short blk_end_pulse_flag : 1 ;
	// if set -- the End Device is capable of having scalars and divisors associated with set #N LP interval data
	unsigned short scalar_divisor_flag_set1 : 1 ;
	unsigned short scalar_divisor_flag_set2 : 1 ;
	unsigned short scalar_divisor_flag_set3 : 1 ;
	unsigned short scalar_divisor_flag_set4 : 1 ;
	// if set -- the End Device is capable of returning extended interval status with LP interval data
	unsigned short extended_int_status_flag : 1 ;
	// if set -- the End Device is capable of returning simple interval status with LP interval data
	unsigned short simple_int_status_flag : 1 ;
	// if set -- the Block-end readings (END_READINGS) are cumulative only across the associated block
	unsigned short blk_end_rd_indicator_flag : 1 ;
	unsigned short filler : 3 ;
} LP_FLAGS_BFLD ;

typedef struct __attribute__((packed)) {
	// if set -- An interval format of UINT8 is capable of being used
	unsigned char inv_uint8_flag : 1 ;
	// if set -- An interval format of UINT16 is capable of being used
	unsigned char inv_uint16_flag : 1 ;
	// if set -- An interval format of UINT32 is capable of being used
	unsigned char inv_uint32_flag : 1 ;
	// if set -- An interval format of INT8 is capable of being used
	unsigned char inv_int8_flag : 1 ;
	// if set --  An interval format of INT16 is capable of being used
	unsigned char inv_int16_flag : 1 ;
	// if set --  An interval format of INT32 is capable of being used
	unsigned char inv_int32_flag : 1 ;
	// if set -- An interval format of NI_FMAT1 is capable of being used
	unsigned char inv_ni_fmat1_flag : 1 ;
	// if set --  An interval format of NI_FMAT2 is capable of being used
	unsigned char inv_ni_fmat2_flag : 1 ;
} LP_FMATS_BFLD ;

typedef struct __attribute__ ((packed)) {
	unsigned short nbr_blks_set ;		/* Maximum number of blocks that can be contained in LP_DATA_SET1_TBL (Table 64) */
	unsigned short nbr_blk_ints_set ;	/* Maximum number of intervals per block that can be contained in LP_DATA_SET1_TBL (Table 64) */
	unsigned char nbr_chns_set ;		/* Maximum number of channels of LP data that can be contained in LP_DATA_SET1_TBL (Table 64) */
	unsigned char max_int_time_set ;	/* Maximum time in minutes for LP interval duration that can be contained in LP_DATA_SET1_TBL (Table 64) */
} LP_DATA_SET_TBL ;

typedef struct __attribute((packed)) {
	unsigned int lp_memory_len ;		/*
											0..4294967295 -- Maximum number of octets of storage available
											for Load Profile data. This reflects the combined
											sizes of tables LP_DATA_SET1_TBL (Table 64), 
											LP_DATA_SET2_TBL (Table 65), LP_DATA_SET3_TBL (Table 66), 
											and LP_DATA_SET4_TBL (Table 67) */
	LP_FLAGS_BFLD lp_flags ;			
	LP_FMATS_BFLD lp_fmats ;
//// actual number of elements depends on if gen_config_tbl.std_tbls_used[lp_data_setX_tbl] is TRUE
	LP_DATA_SET_TBL lp_dstbl[4] ;
} LP_SET_RCD ;

//////////// Table 61 Actual Load Profile Limiting Table
//// ACT_LP_TBL (Table 61) contains actual dimensions for Load Profile data. 
//// This Decade provides for up to four independent sets of Load Profile data.
//// has the same structure as Table 60 above, i.e. LP_SET_RCD

/////////////////// Table 62 Load Profile Control Table
typedef struct __attribute__((packed)) {
	unsigned char end_rdg_flag : 1 ;	// if set -- Channel does have an associated end reading
	unsigned char filler : 7 ;
} LP_CTRL_FLAGS_BFLD ;

typedef union __attribute__((packed)) {
	struct {
		unsigned char source_index ;
	} ;
} SOURCE_SELECT_RCD ;

typedef struct __attribute__((packed)) {
	LP_CTRL_FLAGS_BFLD chnl_flag ;					// Flags associated with a particular channel
	SOURCE_SELECT_RCD lp_source_select ;			// Data source selector of the interval data for a specific channel
	SOURCE_SELECT_RCD end_blk_rdg_source_select ;	// Data source selector of the block end reading for a specific channel
} LP_SOURCE_SEL_RCD ;	// This record stores all information concerning each of the selected channels

typedef union __attribute__((packed)) {
} DATA_SELECTION_RCD ;

typedef enum {
	IFMT_UINT8 = 1,
	IFMT_UINT16 = 2,
	IFMT_UINT32 = 4,
	IFMT_INT8 = 8,
	IFMT_INT16 = 16,
	IFMT_INT32 = 32,
	NIFMT_FMAT1 = 64,
	NIFMT_FMAT2 = 128
} INT_FMT_ENUM ;

typedef LP_SOURCE_SEL_RCD *LP_SEL_SET ; // Array of records that identifies sources of data for each channel of interval data in LP_DATA_SETx_TBL (Table 64)
typedef unsigned char INT_FMT_CDE ;		// Single code selecting the format for all interval data in LP_DATA_SETx_TBL (Table 64). See INT_FMT_ENUM
typedef unsigned short * SCALAR_SET ;	// Array of scalars applied to interval data before recording pulse data in Load Profile set X.
typedef unsigned short * DIVISOR_SET ;	// Array of divisors applied to interval data before recording pulse data in Load Profile set X.
/***
IF GEN_CONFIG_TBL.STD_TBLS_USED[LP_DATA_SET1_TBL] THEN
	LP_SEL_SETx : ARRAY[ACT_LP_TBL.NBR_CHNS_SETx] OF LP_SOURCE_SEL_RCD;
	INT_FMT_CDEx : UINT8;
	IF ACT_LP_TBL.SCALAR_DIVISOR_FLAG_SETx THEN
		SCALARS_SETx : ARRAY[ACT_LP_TBL.NBR_CHNS_SETx] OF UINT16;
		DIVISOR_SETx : ARRAY[ACT_LP_TBL.NBR_CHNS_SETx] OF UINT16;
	END;
END;
***/

/////////////////// Table 63 Load Profile Status Table
typedef struct __attribute__((packed)) {
	unsigned char order : 1 ;			// 0 -- Blocks of Load Profile data are transported in ascending order (N is older than N+1). 
										// 1 -- Blocks of Load Profile data are transported in descending order (N is newer than N+1).
	unsigned char overflow_flag : 1 ;	// 1 -- An attempt was made to enter an interval in a new data block such that the number 
										// of unread blocks exceeded the actual number of possible blocks in Load Profile storage
	unsigned char list_type : 1 ;		// 0 -- FIFO (First In First Out) as placed in Load Profile storage
										// 1 -- Circular list as placed in Load Profile storage
	unsigned char block_inhibit_overflow_flag : 1 ;	// The same value as ACT_LP_TBL.LP_SETn_INHIBIT_OVF_FLAG
	unsigned char interval_order : 1 ;	// 0 -- Intervals in each block of Load Profile are transported in ascending order (N is older than N+1).
										// 1 -- Intervals in each block of Load Profile are transported in descending order (N is newer than N+1)
	unsigned char active_mode_flag : 1 ;// 0 -- Data set is not collecting data
										// 1 -- Data set is collecting data
	unsigned char test_mode : 1 ;		// 0 -- Data set is collecting Metering Mode data
										// 1 -- Data set is collecting Test Mode data
	unsigned char filler : 1 ;	
} LP_SET_STATUS_BFLD ; 

typedef struct __attribute((packed)) {
	LP_SET_STATUS_BFLD lp_set_status_flags ;
	unsigned short nbr_valid_blocks ;			/* Number of valid Load Profile data blocks in Load	Profile data tables 
												LP_DATA_SET1_TBL (Table 64), LP_DATA_SET2_TBL (Table 65), LP_DATA_SET3_TBL (Table 66), and
												LP_DATA_SET4_TBL (Table 67), Load Profile block arrays. The range is zero (meaning no
												data blocks in Load Profile Data Table) to the actual dimension of the number of Load Profile
												data blocks. The block is considered valid when at least one interval is written */
	unsigned short last_block_element ;			/* Array element index of the newest valid data block in the Load Profile data array. This field is
												valid only if NBR_VALID_BLOCKS is greater than zero */
	unsigned int last_block_seq_nbr ;			/* Sequence number of the newest valid data block (LAST_BLOCK_ELEMENT) in the Load
												Profile data array. This sequence number corresponds to the first valid interval entry 
												in the block */
	unsigned short nbr_unread_blocks ;			/* Number of Load Profile blocks that have not been read. This number shall only be updated
												through the invocation of Procedure 04, "Reset List Pointers" or Procedure 05, "Update
												Last Read Entry". */
	unsigned short nbr_valid_int ;				/* Number of valid intervals stored in the newest Load Profile block whose array index is
												LAST_BLOCK_ELEMENT. Valid values are in the range of zero (meaning no interval in the
												array) to the actual dimension of the number of intervals per block */
} LP_SET_STATUS_RCD ;

////typedef struct __attribute((packed)) {
////	// IF GEN_CONFIG_TBL.STD_TBLS_USED[LP_DATA_SET1_TBL] THEN
////	LP_SET_STATUS_RCD lp_status_set1 ;							/* Status information for profile data set 1 */
////	// IF GEN_CONFIG_TBL.STD_TBLS_USED[LP_DATA_SET2_TBL] THEN
////	LP_SET_STATUS_RCD lp_status_set2 ;
////	// IF GEN_CONFIG_TBL.STD_TBLS_USED[LP_DATA_SET3_TBL] THEN
////	LP_SET_STATUS_RCD lp_status_set3 ;
////	// IF GEN_CONFIG_TBL.STD_TBLS_USED[LP_DATA_SET4_TBL] THEN
////	LP_SET_STATUS_RCD lp_status_set4 ;
////} LP_STATUS_RCD ;

/////////////////// Table 64 Load Profile Data Set One Table

/*  The contents of the even and odd channel
status nibbles. See EXTENDED_STATUS_BFLD. */
typedef enum {
	CHSTATUS_NOX =	0, // No exception status indicated.
	CHSTATUS_OFL =	1, // Numeric overflow condition detected.
	CHSTATUS_PAR =	2, // Partial interval due to common state. The cause is indicated in the common status flags.
	CHSTATUS_LNG =	3, // Long interval due to common state. The cause is indicated in the common status flags.
	CHSTATUS_SKP =	4, // Skipped interval due to common state. The cause is indicated in the common status flags. 
					   // The INT_DATA shall be set to zero (0).
	CHSTATUS_TMO =	5, // The interval contains test mode data.
	CHSTATUS_CFG =	6, // The configuration changed during this interval.
	CHSTATUS_STP =	7 // Load profile recording stopped.
	// 8..15 Reserved.
} CHANNEL_STATUS_ENUM ;

/*
Selector for the format of the channel data. Also See LP_CTRL_TBL.INT_FMT_ENUM
*/
typedef union __attribute__ ((packed))
{
	unsigned char item_fmt1 ;
	unsigned short item_fmt2 ;
	unsigned int item_fmt4 ;
	char item_fmt8 ;
	short item_fmt16 ;
	int item_fmt32 ;
	NI_FMT fmt1_item_fmt64 ;
	NI_FMT fmt2_item_fmt128 ;
} INT_FMT_RCD ;

/* 
  This bit field is expected to be used as an entry of an array. The interpretation of the nibbles 
  of each UINT8 member of the array depends on its position index being the first in the collection
  (.lastIndex == 0) or not being the first (.lastIndex > 0).
  */
typedef union __attribute__((packed)) {
	/*
	The index value of this bit field (i.e., the first UINT8) in the containing array is 0,
	implying that this UINT8 contains the extended common status flags (bits 4..7) and the
	extended channel status of channel 1 (bits 0..3)
	*/
	struct __attribute__((packed)) {
		unsigned char x_common_dst_flag : 1 ;
		unsigned char x_common_power_fail_flag : 1 ;
		unsigned char x_common_clock_set_fwd_flag : 1 ;
		unsigned char x_common_clock_set_bkwd_flag: 1 ;
		unsigned char common_dst_flag : 1 ;					/* Daylight saving time status change flag */
		unsigned char common_power_fail_flag : 1 ;			/* Power fail detection flag. If the power fail
															condition spans an entire interval duration then
															the value of INT_DATA shall be set to zero (0)
															in all applicable intervals of the affected block
															until the power is restored or the block is closed,
															whichever occurs first */
		unsigned char common_clock_set_fwd_flag : 1 ;		/* Clock set forward indication flag. Setting the
															clock forward may cause intervals and blocks to
															be partial or skipped. Time reset across a block
															boundary may result in the closure of that active
															block and setting all remaining interval
															INT_DATA to zero (0) asserting the COMMON_CLOCK_SET_FWD_FLAG
															repeatedly to TRUE and the channel’s status to
															skipped interval due to common state (4). */
		unsigned char common_clock_set_bkwd_flag: 1 ;		/* Clock set backward indication flag */
	} lizero ;

	/*
	This is the description of the extended status array indices that are greater than zero.
	Each UINT8 contains data for of even channels 2,4,6 and odd channels 3,5,7 … through
	channel NBR_CHNS_SETn, where n is the load profile set number. The last
	CHANNEL_STATUS_ODD bits are FILL bits, when the number of channels is even.
	*/
	struct __attribute__((packed)) {
		unsigned char channel_status_odd : 4 ;				/* The contents of the odd channel status nibble. See CHANNEL_STATUS_ENUM */
		unsigned char channel_status_even : 4 ;				/* The contents of the even channel status nibble. See CHANNEL_STATUS_ENUM */
	} linonzero ;	
} EXTENDED_STATUS_BFLD ; 

/*
TYPE INT_SET1_RCD = PACKED RECORD
	IF ACT_LP_TBL.EXTENDED_INT_STATUS_FLAG THEN
		EXTENDED_INT_STATUS : ARRAY[(ACT_LP_TBL.NBR_CHNS_SET1/2)+1] OF EXTENDED_STATUS_BFLD;
	END;
	INT_DATA : ARRAY[ACT_LP_TBL.NBR_CHNS_SET1] OF INT_FMT1_RCD;
END;
*/
EXTENDED_STATUS_BFLD *extended_int_status ;
INT_FMT_RCD *int_data ;

/*
TYPE READINGS_RCD = PACKED RECORD
	IF ACT_LP_TBL.BLK_END_READ_FLAG THEN
		BLOCK_END_READ : NI_FMAT1;
	END;
	IF ACT_LP_TBL.BLK_END_PULSE_FLAG THEN
		BLOCK_END_PULSE : UINT32;
	END;
END;
*/
typedef union __attribute__ ((packed)) {
	struct {
		NI_FMT block_end_read ;
	} ;
	struct {
		unsigned int block_end_pulse ;
	} ;
} READINGS_RCD ;


/***
TYPE LP_BLK1_DAT_RCD = PACKED RECORD
	BLK_END_TIME : STIME_DATE;
	END_READINGS : ARRAY[ACT_LP_TBL.NBR_CHNS_SET1] OF READINGS_RCD;
	IF ACT_LP_TBL.SIMPLE_INT_STATUS_FLAG THEN
		SIMPLE_INT_STATUS : SET((ACT_LP_TBL.NBR_BLK_INTS_SET1+7)/8);
	END;
	LP_INT : ARRAY[ACT_LP_TBL.NBR_BLK_INTS_SET1] OF INT_SET1_RCD;
END;
***/
typedef struct __attribute__ ((packed)) {
	STIME_DATE blk_end_time ;
	READINGS_RCD end_readings ;
} LP_BLK_DAT_RCD ;
void *simple_int_status ;
////INT_SET_RCD *lp_int ;

/***
TYPE LP_DATA_SET1_RCD = PACKED RECORD
	LP_DATA_SET1 : ARRAY[ACT_LP_TBL.NBR_BLKS_SET1] OF LP_BLK1_DAT_RCD;
END;
***/
typedef struct __attribute__((packed)) {
	LP_BLK_DAT_RCD lp_data_set[1] ;
} LP_DATA_SET_RCD ;

//////////////////////////////////////////////////////

typedef struct {
	int code ;
	unsigned char *descr ;
} lookup_t ;

extern unsigned char wrongcode[] ;
extern char outdir[] ;

extern unsigned char *dsclookup(int n) ;
extern int parse_request(unsigned char *preq, int len, int vlevel) ;
extern int parse_response(unsigned char *prsp, int len, int mrc, int vlevel) ; // mrc -- matching request code(zero means N/A)

extern void dirunlink(char *pdir, char *pfname) ;
extern FILE *fdiropen(char *pdir, char *pfname, char *pmode) ;

extern int savetable(int tableid, unsigned char *pdata, int length) ;
#endif
