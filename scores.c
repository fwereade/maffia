/*
 *  scores.c
 *  nibMAFF
 *
 *  Created by William Reade on Thu Oct 17 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include "mafftypes.h"
#include "scores.h"

extern GlobalStuff *g;

extern void CleanUp(bool instaQuit);

void LoadHighScores(void)
{
    CFIndex		index = 0;
    CFArrayRef		theScoreArray;
    CFArrayRef		theKilledArray;
    CFArrayRef		theMultArray;
    CFArrayRef		theJuggleArray;
    CFArrayRef		theScoreScoreArray;
    CFArrayRef		theScoreKilledArray;
    CFArrayRef		theScoreMultArray;
    CFArrayRef		theScoreJuggleArray;
    CFStringRef		theName;
    CFNumberRef		theNumber;
    
    theScoreArray = CFPreferencesCopyAppValue(CFSTR("scoreNames"), kCFPreferencesCurrentApplication);
    
    if (!theScoreArray)
        DefaultHighScores();
    else
    {
        theKilledArray = CFPreferencesCopyAppValue(CFSTR("killedNames"), kCFPreferencesCurrentApplication);
        theMultArray = CFPreferencesCopyAppValue(CFSTR("multNames"), kCFPreferencesCurrentApplication);
        theJuggleArray = CFPreferencesCopyAppValue(CFSTR("juggleNames"), kCFPreferencesCurrentApplication);
        
        theScoreScoreArray = CFPreferencesCopyAppValue(CFSTR("scoreScores"), kCFPreferencesCurrentApplication);
        theScoreKilledArray = CFPreferencesCopyAppValue(CFSTR("killedScores"), kCFPreferencesCurrentApplication);
        theScoreMultArray = CFPreferencesCopyAppValue(CFSTR("multScores"), kCFPreferencesCurrentApplication);
        theScoreJuggleArray = CFPreferencesCopyAppValue(CFSTR("juggleScores"), kCFPreferencesCurrentApplication);
        
        g->scores.lastName = CFPreferencesCopyAppValue(CFSTR("lastName"), kCFPreferencesCurrentApplication);
        
        while (index < 10)
        {
            theName = CFArrayGetValueAtIndex(theScoreArray, index);
            CFRetain(theName);
            g->scores.scoreName[index] = theName;
            
            theName = CFArrayGetValueAtIndex(theKilledArray, index);
            CFRetain(theName);
            g->scores.killedName[index] = theName;
            
            theName = CFArrayGetValueAtIndex(theMultArray, index);
            CFRetain(theName);
            g->scores.multName[index] = theName;
            
            theName = CFArrayGetValueAtIndex(theJuggleArray, index);
            CFRetain(theName);
            g->scores.juggleName[index] = theName;
            
            theNumber = CFArrayGetValueAtIndex(theScoreScoreArray, index);
            CFNumberGetValue(theNumber, kCFNumberLongType, &(g->scores.score[index]));
            
            theNumber = CFArrayGetValueAtIndex(theScoreKilledArray, index);
            CFNumberGetValue(theNumber, kCFNumberLongType, &(g->scores.sheepKilled[index]));
            
            theNumber = CFArrayGetValueAtIndex(theScoreMultArray, index);
            CFNumberGetValue(theNumber, kCFNumberLongType, &(g->scores.highestMultiplier[index]));
            
            theNumber = CFArrayGetValueAtIndex(theScoreJuggleArray, index);
            CFNumberGetValue(theNumber, kCFNumberLongType, &(g->scores.longestJuggle[index]));
            
            index++;
        }
        
        CFRelease(theScoreArray);
        CFRelease(theKilledArray);
        CFRelease(theMultArray);
        CFRelease(theJuggleArray);
        CFRelease(theScoreScoreArray);
        CFRelease(theScoreKilledArray);
        CFRelease(theScoreMultArray);
        CFRelease(theScoreJuggleArray);
    }
    
    g->scores.lastLookedAt = 1;
    
    g->scores.lastScoreIndex = -1;
    g->scores.lastKilledIndex = -1;
    g->scores.lastMultIndex = -1;
    g->scores.lastJuggleIndex = -1;
}

void SaveHighScores(void)
{
    CFIndex			index = 0;
    CFMutableArrayRef		theScoreArray;
    CFMutableArrayRef		theKilledArray;
    CFMutableArrayRef		theMultArray;
    CFMutableArrayRef		theJuggleArray;
    CFMutableArrayRef		theScoreScoreArray;
    CFMutableArrayRef		theScoreKilledArray;
    CFMutableArrayRef		theScoreMultArray;
    CFMutableArrayRef		theScoreJuggleArray;
    CFNumberRef			theNumber[40];
    
    theScoreArray = CFArrayCreateMutable(NULL, 10, NULL);
    theKilledArray = CFArrayCreateMutable(NULL, 10, NULL);
    theMultArray = CFArrayCreateMutable(NULL, 10, NULL);
    theJuggleArray = CFArrayCreateMutable(NULL, 10, NULL);
    theScoreScoreArray = CFArrayCreateMutable(NULL, 10, NULL);
    theScoreKilledArray = CFArrayCreateMutable(NULL, 10, NULL);
    theScoreMultArray = CFArrayCreateMutable(NULL, 10, NULL);
    theScoreJuggleArray = CFArrayCreateMutable(NULL, 10, NULL);
    
    while (index < 10)
    {
        CFArrayAppendValue(theScoreArray, g->scores.scoreName[index]);
        CFArrayAppendValue(theKilledArray, g->scores.killedName[index]);
        CFArrayAppendValue(theMultArray, g->scores.multName[index]);
        CFArrayAppendValue(theJuggleArray, g->scores.juggleName[index]);
        
        theNumber[index * 4] = CFNumberCreate(NULL, kCFNumberLongType, &g->scores.score[index]);
        CFArrayAppendValue(theScoreScoreArray, theNumber[index * 4]);
        
        theNumber[index * 4 + 1] = CFNumberCreate(NULL, kCFNumberLongType, &g->scores.sheepKilled[index]);
        CFArrayAppendValue(theScoreKilledArray, theNumber[index * 4 + 1]);
        
        theNumber[index * 4 + 2] = CFNumberCreate(NULL, kCFNumberLongType, &g->scores.highestMultiplier[index]);
        CFArrayAppendValue(theScoreMultArray, theNumber[index * 4 + 2]);
        
        theNumber[index * 4 + 3] = CFNumberCreate(NULL, kCFNumberLongType, &g->scores.longestJuggle[index]);
        CFArrayAppendValue(theScoreJuggleArray, theNumber[index * 4 + 3]);
        
        index++;
    }
    
    CFPreferencesSetAppValue(CFSTR("scoreNames"), theScoreArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("killedNames"), theKilledArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("multNames"), theMultArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("juggleNames"), theJuggleArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("scoreScores"), theScoreScoreArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("killedScores"), theScoreKilledArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("multScores"), theScoreMultArray, kCFPreferencesCurrentApplication);
    CFPreferencesSetAppValue(CFSTR("juggleScores"), theScoreJuggleArray, kCFPreferencesCurrentApplication);
    
    CFPreferencesSetAppValue(CFSTR("lastName"), g->scores.lastName, kCFPreferencesCurrentApplication);

    CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    
    index = 0;
    while(index < 40)
    {
        CFRelease(theNumber[index]);
        index++;
    }
    
    CFRelease(theScoreArray);
    CFRelease(theKilledArray);
    CFRelease(theMultArray);
    CFRelease(theJuggleArray);
    CFRelease(theScoreScoreArray);
    CFRelease(theScoreKilledArray);
    CFRelease(theScoreMultArray);
    CFRelease(theScoreJuggleArray);
}


void DefaultHighScores(void)
{
    CFStringRef		theName;
    short		i = 0;
    unsigned long	scoreMax = 10000000;
    long		killedMax = 400;	// 401 totalÉ?
    short		multMax = 20;
    short		juggleMax = 250;
    bool		scoreFive = FALSE;
    
    theName = CFSTR("Wibble");
    
    g->scores.lastName = CFStringCreateCopy(NULL, theName);
    
    if (!g->scores.lastName)
        CleanUp(TRUE);
    
    while (i < 10)
    {
        g->scores.scoreName[i] = CFStringCreateCopy(NULL, theName);
        g->scores.killedName[i] = CFStringCreateCopy(NULL, theName);
        g->scores.multName[i] = CFStringCreateCopy(NULL, theName);
        g->scores.juggleName[i] = CFStringCreateCopy(NULL, theName);
        
        g->scores.score[i] = scoreMax;
        if (scoreFive)
            scoreMax /= 5;
        else
            scoreMax /= 2;
        
        scoreFive = !scoreFive;
        
        g->scores.sheepKilled[i] = killedMax - (i * killedMax / 10);
        g->scores.highestMultiplier[i] = multMax - (i * multMax / 10);
        g->scores.longestJuggle[i] = juggleMax - (i * juggleMax / 10);
        
        i++;
    }
    
    g->scores.lastScoreIndex = -1;
    g->scores.lastKilledIndex = -1;
    g->scores.lastMultIndex = -1;
    g->scores.lastJuggleIndex = -1;
    
    SaveHighScores();
    
    g->theScoreStuff.score = 0;
    g->theScoreStuff.highestMultiplier = 0;
    g->theScoreStuff.longestJuggle = 0;
    g->theScoreStuff.sheepKilledTotal = 0;
}

bool NewHighScore(void)
{
    if (g->theScoreStuff.score > g->scores.score[9])
        return TRUE;
    
    if (g->theScoreStuff.sheepKilledTotal > g->scores.sheepKilled[9])
        return TRUE;
    
    if (g->theScoreStuff.highestMultiplier > g->scores.highestMultiplier[9])
        return TRUE;
    
    if (g->theScoreStuff.longestJuggle > g->scores.longestJuggle[9])
        return TRUE;
    
    return FALSE;
}

void UpdateHighScores(void)
{
    ControlID		cntlid;
    ControlRef		theControl;
    Size		theSize;
    Size		outSize;
    unsigned char	theString[255];
    CFStringRef		theCFString;
    short		i = 9;
    
    g->scores.lastScoreIndex = -1;
    g->scores.lastKilledIndex = -1;
    g->scores.lastMultIndex = -1;
    g->scores.lastJuggleIndex = -1;
    
    cntlid.signature = 'blob';
    cntlid.id = 132;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    
    GetControlDataSize(theControl, kControlEditTextPart, kControlEditTextTextTag, &theSize);
    
    if (theSize > 255)
        theSize = 255;
    
    GetControlData(theControl, kControlEditTextPart, kControlEditTextTextTag, theSize, theString, &outSize);
    
    if (outSize < 255)
        theString[outSize] = 0;
    else
        theString[254] = 0;
    
    if (outSize == 0)
        theCFString = CFStringCreateCopy(NULL, CFSTR("Anonymous"));
    else
        theCFString = CFStringCreateWithCString(NULL, theString, GetApplicationTextEncoding());
    
    CFRelease(g->scores.lastName);
    g->scores.lastName = theCFString;
    
    while (i >= 0)
    {
        if (g->theScoreStuff.score > g->scores.score[i])
        {
            g->scores.lastScoreIndex = i;
            
            if (i < 9)
            {
                CFRelease(g->scores.scoreName[i+1]);
                g->scores.scoreName[i+1] = g->scores.scoreName[i];
                g->scores.scoreName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.score[i+1] = g->scores.score[i];
                g->scores.score[i] = g->theScoreStuff.score;
            }
            else
            {
                CFRelease(g->scores.scoreName[i]);
                g->scores.scoreName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.score[i] = g->theScoreStuff.score;
            }
        }
        
        if (g->theScoreStuff.sheepKilledTotal > g->scores.sheepKilled[i])
        {
            g->scores.lastKilledIndex = i;
            
            if (i < 9)
            {
                CFRelease(g->scores.killedName[i+1]);
                g->scores.killedName[i+1] = g->scores.killedName[i];
                g->scores.killedName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.sheepKilled[i+1] = g->scores.sheepKilled[i];
                g->scores.sheepKilled[i] = g->theScoreStuff.sheepKilledTotal;
            }
            else
            {
                CFRelease(g->scores.killedName[i]);
                g->scores.killedName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.sheepKilled[i] = g->theScoreStuff.sheepKilledTotal;
            }
        }
        
        if (g->theScoreStuff.highestMultiplier > g->scores.highestMultiplier[i])
        {
            g->scores.lastMultIndex = i;
            
            if (i < 9)
            {
                CFRelease(g->scores.multName[i+1]);
                g->scores.multName[i+1] = g->scores.multName[i];
                g->scores.multName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.highestMultiplier[i+1] = g->scores.highestMultiplier[i];
                g->scores.highestMultiplier[i] = g->theScoreStuff.highestMultiplier;
            }
            else
            {
                CFRelease(g->scores.multName[i]);
                g->scores.multName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.highestMultiplier[i] = g->theScoreStuff.highestMultiplier;
            }
        }
        
        if (g->theScoreStuff.longestJuggle > g->scores.longestJuggle[i])
        {
            g->scores.lastJuggleIndex = i;
            
            if (i < 9)
            {
                CFRelease(g->scores.juggleName[i+1]);
                g->scores.juggleName[i+1] = g->scores.juggleName[i];
                g->scores.juggleName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.longestJuggle[i+1] = g->scores.longestJuggle[i];
                g->scores.longestJuggle[i] = g->theScoreStuff.longestJuggle;
            }
            else
            {
                CFRelease(g->scores.juggleName[i]);
                g->scores.juggleName[i] = CFStringCreateCopy(NULL, g->scores.lastName);
                
                g->scores.longestJuggle[i] = g->theScoreStuff.longestJuggle;
            }
        }
        
        i--;
    }
    
    SaveHighScores();
    
    g->theScoreStuff.score = 0;
    g->theScoreStuff.highestMultiplier = 0;
    g->theScoreStuff.longestJuggle = 0;
    g->theScoreStuff.sheepKilledTotal = 0;
}

void UnloadHighScores(void)
{
    short	i = 0;
    
    while (i<10)
    {
        CFRelease(g->scores.scoreName[i]);
        CFRelease(g->scores.killedName[i]);
        CFRelease(g->scores.multName[i]);
        CFRelease(g->scores.juggleName[i]);
        
        i++;
    }
    
    CFRelease(g->scores.lastName);
    
    CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    
}

