/*
 * diag.c
 *
 *  Created on: Mar 22, 2014
 *      Author: andy
 */

#include <stdio.h>

#include "diag.h"

void DumpHex(unsigned char *p, int length)
{
	int          k, i;

	for (k = 0; k < length; k++)
	{
		// if new line, print it
		if ((k & 0x0f) == 0) {
			if (k>0) {
				TRACE("\n") ;
			}
			TRACE("  ");
		}

		// print element
		TRACE("%02x ", p[k] & 0xff);

		// check if at end of line
		if ((k & 0x0f) == 0x0f)
		{
			// print "ascii" version at end
			TRACE("     ");
			for (i = k - 0x0f; i <= k; i++)
			{
				if ((p[i] & 0xff) > 0x1f)
					TRACE("%c", p[i] & 0xff);
				else
					TRACE(".");
			}
		}

		// check for last line
		if ((k == length - 1) && ((k & 0x0f) != 0x0f))
		{
			// print spaces for remaining hex digits
			for (i = k & 0x0f; i < 0x0f; i++)
				TRACE("   ");

			// print spacing for "ascii" version at end
			TRACE("     ");

			// print ascii for last line
			for (i = k - (k & 0x0f); i <= k; i++)
			{
				if ((p[i] & 0xff) > 0x1f)
					TRACE("%c", p[i] & 0xff);
				else
					TRACE(".");
			}

			// Print end of line
			TRACE("\n");

		}
	}
}

//------------------------------------------------------------------------
// void ShowPkt(unsigned char * s, unsigned char *  pPkt, UINT16 count)
//	Dumps a packet with a time stamp and description string
//------------------------------------------------------------------------
//
void ShowPkt(unsigned char *s, unsigned char *pPkt, int count, unsigned int ticCount)
{
	TRACE("%s, Time: %d",
		   s,
		   ticCount);
	DumpHex(pPkt, count);
}

