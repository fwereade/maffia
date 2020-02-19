/*
 *  voting.c
 *  MAFFia
 *
 *  Created by William Reade on Sat Nov 02 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include "voting.h"


bool InVotingPeriod(void)
{
    bool	inPeriod = FALSE;
    DateTimeRec periodStart, periodEnd;
    UInt32	startSecs, endSecs, nowSecs;	// freudian variables? god… YES THEY ARE! … … disturbing
    
    periodStart.year = 2002;
    periodStart.month = 11;
    periodStart.day = 18;
    periodStart.hour = 0;
    periodStart.minute = 0;
    periodStart.second = 0;
    
    periodEnd.year = 2002;
    periodEnd.month = 11;
    periodEnd.day = 24;
    periodEnd.hour = 23;
    periodEnd.minute = 59;
    periodEnd.second = 59;
    
    DateToSeconds(&periodStart, &startSecs);
    DateToSeconds(&periodEnd, &endSecs);
    GetDateTime(&nowSecs);
    
    if ( (startSecs <= nowSecs) && (nowSecs <= endSecs) )
        inPeriod = TRUE;
    
    return inPeriod;
}


