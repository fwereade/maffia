/*
 *  graphics.c
 *  nibMAFF
 *
 *  Created by wibble on Sun Sep 08 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

#include "graphics.h"
#include "mafftypes.h"

extern void StateSwitch(short theState);
extern void NextLevel(void);
extern void EndGame(bool showNonHighScore);
extern GlobalStuff *g;
extern void PlaySound(short channel, Handle sound);
extern bool InVotingPeriod(void);
extern short GetChainLevel(void);

void DrawInterface (GWorldPtr theGWorld)
{
    PixMapHandle	srcPixMap, dstPixMap;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    Rect		srcBounds, dstBounds;
    
    srcPixMap = GetGWorldPixMap(g->interfaceBackGWorld);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcBounds);
    
    GetGWorld(&storePort,&storeDevice);
    SetGWorld(theGWorld, NULL);
    
    LockPixels(srcPixMap);
    LockPixels(dstPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcBounds,
                &srcBounds,
                srcCopy,
                NULL);
    
    UnlockPixels(srcPixMap);
    
    srcPixMap = GetGWorldPixMap(g->interfaceButtons);
    
    GetPixBounds(srcPixMap, &srcBounds);
    dstBounds = srcBounds;
    OffsetRect(&dstBounds, 210,110);
    
    LockPixels(srcPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcBounds,
                &dstBounds,
                srcCopy,
                NULL);
    
    UnlockPixels(srcPixMap);
    
    // that was the background + basic buttons, now the voting reminder
    
    if (InVotingPeriod())
    {
        srcPixMap = GetGWorldPixMap(g->voteGWorld);
        
        GetPixBounds(srcPixMap, &srcBounds);
        dstBounds = srcBounds;
        OffsetRect(&dstBounds, 458, 10);
        
        LockPixels(srcPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &srcBounds,
                    &dstBounds,
                    srcCopy,
                    NULL);
        
        dstBounds = srcBounds;
        OffsetRect(&dstBounds, 10, 10);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &srcBounds,
                    &dstBounds,
                    srcCopy,
                    NULL);
        
        UnlockPixels(srcPixMap);
    }
    
    // and now the fancy button stuff
    
    if (g->mouseOverOption)
    {
        if (g->clickedOnOption)
        {
            if (g->clickedOnOption == g->mouseOverOption)
            {
                
                srcPixMap = GetGWorldPixMap(g->interfaceClick);
                
                SetRect(&srcBounds, 0, (g->mouseOverOption - 1) * 55, 200, g->mouseOverOption * 55);
                dstBounds = srcBounds;
                OffsetRect(&dstBounds, 210,110);
                
                LockPixels(srcPixMap);
                
                CopyBits(	(BitMap *)*srcPixMap,
                                (BitMap *)*dstPixMap,
                                &srcBounds,
                                &dstBounds,
                                srcCopy,
                                NULL);
                
                UnlockPixels(srcPixMap);
                
            }
        }
        else
        {		
            srcPixMap = GetGWorldPixMap(g->interfaceRollover);
            
            SetRect(&srcBounds, 0, (g->mouseOverOption - 1) * 55, 200, g->mouseOverOption * 55);
            dstBounds = srcBounds;
            OffsetRect(&dstBounds, 210,110);
            
            LockPixels(srcPixMap);
            
            CopyBits(	(BitMap *)*srcPixMap,
                        (BitMap *)*dstPixMap,
                        &srcBounds,
                        &dstBounds,
                        srcCopy,
                        NULL);
            
            UnlockPixels(srcPixMap);
            
        }
    }
    
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort,storeDevice);
}





void InitSheepGWorld (GWorldPtr *srcGWorld, GWorldPtr *dstGWorld)
{
	
	PixMapHandle	srcPixMap, dstPixMap;
	GDHandle		storeDevice;
	CGrafPtr		storePort;
	Rect			copyRect = g->theSheepType.deadBounds;
	
	
	GetGWorld(&storePort, &storeDevice);
	
	srcPixMap = GetGWorldPixMap(*srcGWorld);
	dstPixMap = GetGWorldPixMap(*dstGWorld);
	LockPixels(srcPixMap);
	LockPixels(dstPixMap);
	SetGWorld(*dstGWorld, NULL);
	CopyBits(	(BitMap *)*srcPixMap,
				(BitMap *)*dstPixMap,
				&copyRect,
				&copyRect,
				srcCopy,
				NULL);
	UnlockPixels(srcPixMap);
	UnlockPixels(dstPixMap);
	
	SetGWorld(storePort,storeDevice);
}





void DrawGWorldToWindow (GWorldPtr theGWorld, WindowRef theWindow)
{
	PixMapHandle		srcPixMap, dstPixMap;
        CGrafPtr		winPort, storePort;
        GDHandle		winDevice, storeDevice;
	Rect			srcBounds, dstBounds;
	
	srcPixMap = GetGWorldPixMap(theGWorld);
	GetPixBounds(srcPixMap, &srcBounds);
	LockPixels(srcPixMap);
        
        dstBounds = srcBounds;
        if (g->pref.border && theWindow == g->theWindow)
            OffsetRect(&dstBounds, BORDER_WIDTH, BORDER_HEIGHT);
        
        winPort = GetWindowPort(theWindow);
        dstPixMap = GetPortPixMap(winPort);
        LockPixels(dstPixMap);
        
        winDevice = GetMainDevice();
        
        GetGWorld(&storePort, &storeDevice);
        SetGWorld(winPort, winDevice);
        
	CopyBits((BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcBounds,
                &dstBounds,
                srcCopy,
                NULL);
	UnlockPixels(srcPixMap);
        UnlockPixels(dstPixMap);
        
        SetGWorld(storePort, storeDevice);
}

void DrawBorder(void)
{
    CGrafPtr		storePort;
    GDHandle		storeDevice;
    Rect		boundsRect;
    
    if (g->pref.border)
    {
        GetGWorld(&storePort, &storeDevice);
        SetPortWindowPort(g->theWindow);
        
        GetWindowBounds(g->theWindow, kWindowContentRgn, &boundsRect);
        OffsetRect(&boundsRect, -boundsRect.left, -boundsRect.top);
        
        ForeColor(blackColor);
        PaintRect(&boundsRect);
        
        SetGWorld(storePort, storeDevice);
    }
}

void ClearGWorld (GWorldPtr theGWorld, short colour)
{
    Rect		worldBounds;
    PixMapHandle	tempPixMap;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    
    tempPixMap = GetGWorldPixMap(theGWorld);
    GetPixBounds(tempPixMap, &worldBounds);
    
    GetGWorld(&storePort,&storeDevice);
    SetGWorld(theGWorld, NULL);
    
    ForeColor(blackColor);
    BackColor(colour);
    EraseRect(&worldBounds);
    BackColor(whiteColor);
    
    SetGWorld(storePort,storeDevice);
}


void DrawBackground (GWorldPtr theGWorld)
{
    PixMapHandle	srcPixMap, dstPixMap;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    Rect		bounds;
    
    srcPixMap = GetGWorldPixMap(g->theLevel->theBackground->theGWorld);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &bounds);
    
    GetGWorld(&storePort,&storeDevice);
    SetGWorld(theGWorld, NULL);
    
    LockPixels(srcPixMap);
    LockPixels(dstPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &bounds,
                &bounds,
                srcCopy,
                NULL);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort,storeDevice);
}

void DrawStuff (GWorldPtr theGWorld)
{
    short i = 5;
    
    while (i >= 0)
    {
        DrawScenery(theGWorld, i);
        DrawSheep(theGWorld, i);
        i--;
    }
}

void DrawSheep (GWorldPtr theGWorld, short layer)
{
    PixMapHandle	srcPixMap, mskPixMap, dstPixMap;
    Rect		srcRect, dstRect;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    SheepToken		*thisSheep;
    
    if (g->baseSheep)
    {
        thisSheep = g->baseSheep;
        
        GetGWorld(&storePort,&storeDevice);
        SetGWorld(theGWorld, NULL);
        
        dstPixMap = GetGWorldPixMap(theGWorld);
        LockPixels(dstPixMap);
        
        while (thisSheep)
        {
            if (thisSheep->layer == layer)
            {
                if (!thisSheep->timesShot)
                {    
                    if (thisSheep->frame == 1)
                    {
                        if (thisSheep->velocity.x < 0)
                        {
                            srcPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunLeftA);
                            mskPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunLeftMaskA);
                            GetPixBounds(srcPixMap, &srcRect);
                        }else{
                            srcPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunRightA);
                            mskPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunRightMaskA);
                            
                            GetPixBounds(srcPixMap, &srcRect);
                        }
                    }
                    else
                    {
                        if (thisSheep->velocity.x < 0)
                        {
                            srcPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunLeftB);
                            mskPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunLeftMaskB);
                            GetPixBounds(srcPixMap, &srcRect);
                        }else{
                            srcPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunRightB);
                            mskPixMap = GetGWorldPixMap(g->theSheepType.liveSpriteRunRightMaskB);
                            GetPixBounds(srcPixMap, &srcRect);
                        }
                    }
                }
                else
                {
                    srcPixMap = GetGWorldPixMap(thisSheep->deadSprite);
                    mskPixMap = GetGWorldPixMap(thisSheep->deadSpriteMaskWithOutline);
                    srcRect = thisSheep->deadBounds;
                    
                }
                
                dstRect.left = (short)thisSheep->position.x - (short)srcRect.right/2;		// try taking the (short)s out
                dstRect.right = (short)thisSheep->position.x + (short)srcRect.right/2;		// for a rather odd bug ;)
                dstRect.top = (short)thisSheep->position.y - (short)srcRect.bottom/2;
                dstRect.bottom = (short)thisSheep->position.y + (short)srcRect.bottom/2;
                
                if (srcRect.right & 1) dstRect.right++;
                if (srcRect.bottom & 1) dstRect.bottom++;
                
                if (dstRect.left < 0)
                {
                    srcRect.left -= dstRect.left;
                    dstRect.left = 0;
                }
                else if (dstRect.right > 620)
                {
                    srcRect.right -= dstRect.right - 620;
                    dstRect.right = 620;
                }
                
                if (dstRect.top < 0)
                {
                    srcRect.top -= dstRect.top;
                    dstRect.top = 0;
                }
                else if (dstRect.bottom > 420)
                {
                    srcRect.bottom -= dstRect.bottom - 420;
                    dstRect.bottom = 420;
                }
                
                LockPixels(srcPixMap);
                LockPixels(mskPixMap);
                
                CopyMask(   (BitMap *)*srcPixMap,
                            (BitMap *)*mskPixMap,
                            (BitMap *)*dstPixMap,	// already locked
                            &srcRect,
                            &srcRect,
                            &dstRect);
                            
                UnlockPixels(srcPixMap);
                UnlockPixels(mskPixMap);
            }
            
            thisSheep = thisSheep->next;
        }
        UnlockPixels(dstPixMap);
        SetGWorld(storePort,storeDevice);
    }
}

void DrawScenery (GWorldPtr theGWorld, short layer)
{
    PixMapHandle	srcPixMap, mskPixMap, dstPixMap;
    Rect			srcRect, dstRect;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    SceneryToken	*thisToken;
    
    if (g->theLevel->baseSceneryToken)
    {
        thisToken = g->theLevel->baseSceneryToken;
        
        while (thisToken)
        {
            if (thisToken->layer == layer)
            {
                srcPixMap = GetGWorldPixMap(thisToken->type->spriteGWorld);
                mskPixMap = GetGWorldPixMap(thisToken->type->maskGWorld);
                
                GetPixBounds(srcPixMap, &srcRect);
                
                dstPixMap = GetGWorldPixMap(theGWorld);
                
                dstRect.left = (short)thisToken->position.x - (short)(srcRect.right/2);
                dstRect.right = (short)thisToken->position.x + (short)(srcRect.right/2);
                dstRect.top = (short)thisToken->position.y - (short)(srcRect.bottom/2);
                dstRect.bottom = (short)thisToken->position.y + (short)(srcRect.bottom/2);
                
                if (srcRect.right & 1) dstRect.right++;
                if (srcRect.bottom & 1) dstRect.bottom++;
                
                LockPixels(srcPixMap);
                LockPixels(mskPixMap);
                LockPixels(dstPixMap);
                
                GetGWorld(&storePort,&storeDevice);
                SetGWorld(theGWorld, NULL);
                
                CopyMask(	(BitMap *)*srcPixMap,
                            (BitMap *)*mskPixMap,
                            (BitMap *)*dstPixMap,
                            &srcRect,
                            &srcRect,
                            &dstRect);
                
                UnlockPixels(srcPixMap);
                UnlockPixels(mskPixMap);
                UnlockPixels(dstPixMap);
                
                SetGWorld(storePort,storeDevice);
            }
            
            thisToken = thisToken->next;
        }
    }
}



void DrawScoreEffects (GWorldPtr theGWorld)
{
    PixMapHandle	srcPixMap, mskPixMap, dstPixMap;
    Rect		srcRect, dstRect, sizeRect;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    ScoreEffect		*thisScoreEffect;
    short		counter, number, frame, totalWidth, spacer, dh, dv, scoreTemp, multiplierTemp;
    
    
    if (g->baseScoreEffect)
    {
        thisScoreEffect = g->baseScoreEffect;
        
        sizeRect = g->theScoreStuff.numberRect;
        
        GetGWorld(&storePort,&storeDevice);
        SetGWorld(theGWorld, NULL);
        
        dstPixMap = GetGWorldPixMap(theGWorld);
        
        LockPixels(dstPixMap);
        
        while (thisScoreEffect)
        {
            if (thisScoreEffect->animationFrame < 38
                || (thisScoreEffect->animationFrame & 1) )
            {
                counter = 0;
                frame = thisScoreEffect->animationFrame;
                scoreTemp = thisScoreEffect->score;
                multiplierTemp = thisScoreEffect->multiplier;
                
                dstRect = sizeRect;
                dh = thisScoreEffect->position.h;
                dv = thisScoreEffect->position.v - sizeRect.bottom/2;
                
                spacer = sizeRect.right + frame;
                totalWidth = thisScoreEffect->totalCharacters * spacer - frame;
                
                OffsetRect(&dstRect, dh - totalWidth / 2, dv - frame * 2);
                
                srcPixMap = GetGWorldPixMap(g->theScoreStuff.scoreGraphics);
                mskPixMap = GetGWorldPixMap(g->theScoreStuff.scoreMask);
                
                LockPixels(srcPixMap);
                LockPixels(mskPixMap);
                
                while (counter < thisScoreEffect->scoreCharacters)
                {
                    number = scoreTemp / pow(10, thisScoreEffect->scoreCharacters - counter - 1);
                    
                    scoreTemp -= number * pow(10, thisScoreEffect->scoreCharacters - counter - 1);
                    
                    srcRect = sizeRect;
                    OffsetRect(&srcRect, sizeRect.right * number, 0);
                    
                    CopyMask(	(BitMap *)*srcPixMap,
                                (BitMap *)*mskPixMap,
                                (BitMap *)*dstPixMap,
                                &srcRect,
                                &srcRect,
                                &dstRect);
                    
                    OffsetRect(&dstRect, spacer, 0);
                    counter++;
                }
                
                UnlockPixels(srcPixMap);
                srcPixMap = GetGWorldPixMap(g->theScoreStuff.multiplierGraphics);
                LockPixels(srcPixMap);
                
                while (counter < thisScoreEffect->totalCharacters)
                {
                    if (counter == thisScoreEffect->scoreCharacters)
                    {
                        number = 10;
                    }
                    else
                    {
                        number = multiplierTemp / pow (10, thisScoreEffect->totalCharacters - counter - 1);
                            
                        multiplierTemp -= number * pow(10, thisScoreEffect->totalCharacters - counter - 1);
                    }
                    
                    srcRect = sizeRect;
                    OffsetRect(&srcRect, sizeRect.right * number, 0);
                    
                    CopyMask((BitMap *)*srcPixMap,
                            (BitMap *)*mskPixMap,
                            (BitMap *)*dstPixMap,
                            &srcRect,
                            &srcRect,
                            &dstRect);
                    
                    OffsetRect(&dstRect, spacer, 0);
                    counter++;
                }
                
                UnlockPixels(srcPixMap);
                UnlockPixels(mskPixMap);
            }
            
            thisScoreEffect = thisScoreEffect->next;
        }
        
        UnlockPixels(dstPixMap);
        
        SetGWorld(storePort, storeDevice);
    }
}

void DrawShotEffects (GWorldPtr theGWorld)
{
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    ShotEffect		*theShot;
    Rect		theRect;
    short		radius, tempRad;
    
    theShot = g->baseShotEffect;
    
    if (theShot)
    {
        GetGWorld(&storePort, &storeDevice);
        SetGWorld(theGWorld, NULL);
        
        while (theShot)
        {
            tempRad = 20 - theShot->animationFrame;
            radius = 21 - (tempRad * tempRad * 0.04);
            
            theRect.left = theShot->position.h - radius;
            theRect.top = theShot->position.v - radius;
            theRect.right = theShot->position.h + radius;
            theRect.bottom = theShot->position.v + radius;
            
            if (theShot->animationFrame < 12 || theShot->animationFrame & 1)
            {
                switch (theShot->type)
                {
                    case kHitNowt:
                        ForeColor(whiteColor);
                        break;
                    
                    case kHitScenery:
                        ForeColor(yellowColor);
                        break;
                    
                    case kHitSheep:
                        ForeColor(redColor);
                        break;
                    
                    default:
                        break;
                }
                
                if (theShot->animationFrame)
                    FrameOval(&theRect);
                else
                    PaintOval(&theRect);
                
                InsetRect(&theRect, 1, 1);
                
                if (theShot->animationFrame)
                {
                    ForeColor(blackColor);
                    FrameOval(&theRect);
                }
                InsetRect(&theRect, -2, -2);
                FrameOval(&theRect);
            }
            
            theShot = theShot->next;
        }
        ForeColor(blackColor);
        SetGWorld(storePort, storeDevice);
    }
}


void DrawScore (GWorldPtr theGWorld)
{
	PixMapHandle		srcPixMap, mskPixMap, dstPixMap;
	Rect			srcRect, dstRect, sizeRect, outlineRect;
	GDHandle		storeDevice;
	CGrafPtr		storePort;
	short			spacer, counter, number, vertCounter;
	unsigned long		scoreTemp;
        short			i, position;
        short			lifeThingSquareMaker;
        
        static unsigned short	lifeTicks = 0;
        
        
	GetGWorld(&storePort,&storeDevice);
	SetGWorld(theGWorld, NULL);
        
	dstPixMap = GetGWorldPixMap(theGWorld);
	LockPixels(dstPixMap);
        
        
        // draw arrows for sheep above top of screen
        
        srcPixMap = GetGWorldPixMap(g->sheepArrowGWorld);
        mskPixMap = GetGWorldPixMap(g->sheepArrowMask);
        
	LockPixels(srcPixMap);
	LockPixels(mskPixMap);
        
        GetPixBounds(srcPixMap, &srcRect);
        
        i = 0;
        
        while (i < g->sheepArrowsThisFrame)
        {
            position = g->sheepArrows[i] - (srcRect.right/2);
            dstRect = srcRect;
            OffsetRect(&dstRect, position, 0);
            
            CopyMask(   (BitMap *)*srcPixMap,
                        (BitMap *)*mskPixMap,
                        (BitMap *)*dstPixMap,
                        &srcRect,
                        &srcRect,
                        &dstRect);
            
            i++;
        }
        
	UnlockPixels(srcPixMap);
	UnlockPixels(mskPixMap);
        
        
        
        // actual score display
        
	
	sizeRect = g->theScoreStuff.numberRect;
	
	dstRect = sizeRect;
	OffsetRect(&dstRect, 508, 44);
	
	counter = 0;
	spacer = sizeRect.right;
	scoreTemp = g->theScoreStuff.score;
	
	srcPixMap = GetGWorldPixMap(g->theScoreStuff.scoreGraphics);
	mskPixMap = GetGWorldPixMap(g->theScoreStuff.scoreMask);
	
	LockPixels(srcPixMap);
	LockPixels(mskPixMap);
	
	while (counter < 9)
	{
            number = scoreTemp / pow(10, 9 - counter - 1);
            scoreTemp -= number * pow(10, 9 - counter - 1);
            
            srcRect = sizeRect;
            OffsetRect(&srcRect, sizeRect.right * number, 0);
            
            CopyMask(   (BitMap *)*srcPixMap,
                        (BitMap *)*mskPixMap,
                        (BitMap *)*dstPixMap,
                        &srcRect,
                        &srcRect,
                        &dstRect);
            
            OffsetRect(&dstRect, spacer, 0);
            
            counter++;
	}
	
	UnlockPixels(srcPixMap);
	UnlockPixels(mskPixMap);
        
        // that was the score, now weapons display.
        
        SetRect(&sizeRect, 492, 6, 516, 30);
        OffsetRect(&sizeRect, (g->theWeapon->weaponID - 1) * 32, 0);
        
        if ( (g->frameTime - g->theWeapon->lastFired) > g->theWeapon->reloadTime * 30 / g->fps ) // turbo mode hack
            ForeColor(whiteColor);
        else
            ForeColor(yellowColor);
        
        PaintOval(&sizeRect);
        ForeColor(blackColor);
        PenSize(2,2);
        FrameOval(&sizeRect);
        PenSize(1,1);
        ForeColor(blackColor);
        
        srcPixMap = GetGWorldPixMap(g->gunsPictGWorld);
        mskPixMap = GetGWorldPixMap(g->gunsPictMask);
	LockPixels(srcPixMap);
	LockPixels(mskPixMap);
        
        GetPixBounds(srcPixMap, &srcRect);
        
        dstRect = srcRect;
        OffsetRect(&dstRect, 490, 4);
        
        CopyMask(   (BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    (BitMap *)*dstPixMap,
                    &srcRect,
                    &srcRect,
                    &dstRect);
        
	UnlockPixels(srcPixMap);
	UnlockPixels(mskPixMap);
        
        
        
        // and now all our lives:
        
        srcPixMap = GetGWorldPixMap(g->lifePictGWorld);
        mskPixMap = GetGWorldPixMap(g->lifePictMask);
	LockPixels(srcPixMap);
	LockPixels(mskPixMap);
        
        GetPixBounds(srcPixMap, &srcRect);
        
        dstRect = srcRect;
        OffsetRect(&dstRect, 8, 8);
        
        lifeTicks++;
        if (g->theScoreStuff.livesLeft == 1)
        {
            if (g->gameState == kPlaying)
            {
                if (lifeTicks < 20 || lifeTicks > 30)
                {
                    
                    outlineRect = dstRect;
                    lifeThingSquareMaker = srcRect.bottom - srcRect.right;
                    InsetRect(&outlineRect, -4, -4);
                    outlineRect.bottom -= lifeThingSquareMaker;
                    OffsetRect(&outlineRect, 1, 0);
                    ForeColor(whiteColor);
                    PaintOval(&outlineRect);
                    PenSize(2,2);
                    ForeColor(blackColor);
                    FrameOval(&outlineRect);
                    PenSize(1,1);
                }
                if (lifeTicks > 30)
                    lifeTicks = 0;
                
                if (lifeTicks == 0 && g->theScoreStuff.livesLeftBeeps < 2)
                {
                    g->theScoreStuff.livesLeftBeeps++;
                    PlaySound(kBleepChannel, g->sounds.lastLife);
                }
            }
        }
        else
            g->theScoreStuff.livesLeftBeeps = 0;
        
        counter = 0;
        vertCounter = 0;
        
        while (counter < g->theScoreStuff.livesLeft)
        {
            // srcRect set ok, dstRect set to first place
            
            CopyMask(   (BitMap *)*srcPixMap,
                        (BitMap *)*mskPixMap,
                        (BitMap *)*dstPixMap,
                        &srcRect,
                        &srcRect,
                        &dstRect);
            
            OffsetRect(&dstRect, srcRect.right + 4, 0);
            
            counter++;
            
            if ( !(counter % 5) )
            {
                vertCounter++;
                dstRect = srcRect;
                OffsetRect(&dstRect, 8 + 7 * vertCounter, 8 + 6 * vertCounter);
            }
        }
        
	UnlockPixels(srcPixMap);
	UnlockPixels(mskPixMap);
        
        // and that's that. unlock destination and we can all go home.
        
	UnlockPixels(dstPixMap);
	
	SetGWorld(storePort, storeDevice);
	
}

void DrawBonus (GWorldPtr theGWorld)
{
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap = NULL, dstPixMap = NULL, mskPixMap = NULL;
    Rect		srcRect, dstRect, sizeRect;
    
    bool		nothingSet;
    
    short		actualTicks;
    
    short		i, digits, startpoint, spacer, chainLevel, chainOffset;
    UInt32		scoreTemp, number;
    
    if (g->frameTime - g->lastBonusTime < BONUS_TIME_LENGTH)
    {
        switch (g->lastBonus)
        {
            case kLambChopBonus:
                
                srcPixMap = GetGWorldPixMap(g->LCBonusGWorld);
                nothingSet = FALSE;
                break;
            
            case kSwissCheeseBonus:
                
                srcPixMap = GetGWorldPixMap(g->SCBonusGWorld);
                nothingSet = FALSE;
                break;
            
            case kShishKebabBonus:
                
                srcPixMap = GetGWorldPixMap(g->SKBonusGWorld);
                nothingSet = FALSE;
                break;
            
            case kHighFlierBonus:
                
                srcPixMap = GetGWorldPixMap(g->HFBonusGWorld);
                nothingSet = FALSE;
                break;
            
            case kJammyDodgerBonus:
                
                srcPixMap = GetGWorldPixMap(g->JDBonusGWorld);
                nothingSet = FALSE;
                break;
            
            case kChainUpBonus:
                
                srcPixMap = GetGWorldPixMap(g->CUBonusGWorld);
                nothingSet = FALSE;
                break;
            
            default:
                nothingSet = TRUE;
                break;
        }
        
        // make it flash as it disappears, if it's there
        
        if (!nothingSet)
        {
            actualTicks = (short)((g->frameTime - g->lastBonusTime) * 30);
            
            if ( actualTicks > 60 && (actualTicks & 1) )
                nothingSet = TRUE;
        }
        
        
        // if we want to show the bonus:
        
        if (!nothingSet)
        {
            GetGWorld(&storePort, &storeDevice);
            SetGWorld(theGWorld, NULL);
            
            GetPixBounds(srcPixMap, &srcRect);
            dstRect = srcRect;
            OffsetRect(&dstRect, (620 - srcRect.right)/2, 8);
            
            mskPixMap = GetGWorldPixMap(g->bonusMask);
            dstPixMap = GetGWorldPixMap(theGWorld);
            
            LockPixels(srcPixMap);
            LockPixels(mskPixMap);
            LockPixels(dstPixMap);
            
            CopyMask(   (BitMap *)*srcPixMap,
                        (BitMap *)*mskPixMap,
                        (BitMap *)*dstPixMap,
                        &srcRect,
                        &srcRect,
                        &dstRect);
            
            UnlockPixels(srcPixMap);
            UnlockPixels(mskPixMap);
            
            /////////////////////   draw numbers if appropriate
            
            if (g->lastBonusData)  // will be 0 if lambChop bonus only, full chain score otherwise
            {
                scoreTemp = g->lastBonusData;
                sizeRect = g->theScoreStuff.numberRect;
                
                if (actualTicks <= 3 && actualTicks & 1)
                    srcPixMap = GetGWorldPixMap(g->theScoreStuff.scoreGraphics);
                else
                    srcPixMap = GetGWorldPixMap(g->theScoreStuff.multiplierGraphics);
                
                
                mskPixMap = GetGWorldPixMap(g->theScoreStuff.scoreMask);
                
                LockPixels(srcPixMap);
                LockPixels(mskPixMap);
                
                digits = 0;
                while (pow(10, digits) <= scoreTemp)	// ooh, amn't I clever.
                    digits++;
                
                spacer = sizeRect.right + 7 - digits;
                
                if (digits & 1)
                    startpoint = 310 - sizeRect.right/2 - spacer*(digits/2);
                else
                    startpoint = 310 + (spacer - sizeRect.right)/2 - spacer*(digits/2);
                
                dstRect = sizeRect;
                OffsetRect(&dstRect, startpoint, 50);
                
                i = 0;
                while (i < digits)
                {
                    number = scoreTemp / pow(10, digits - i - 1);
                    scoreTemp -= number * pow(10, digits - i - 1);
                    
                    srcRect = sizeRect;
                    OffsetRect(&srcRect, sizeRect.right * number, 0);
                    
                    CopyMask(   (BitMap *)*srcPixMap,
                                (BitMap *)*mskPixMap,
                                (BitMap *)*dstPixMap,
                                &srcRect,
                                &srcRect,
                                &dstRect);
                    
                    OffsetRect(&dstRect, spacer, 0);
                
                    i++;
                }
                
                UnlockPixels(srcPixMap);
                UnlockPixels(mskPixMap);
                
                // end of numbers
            }
            
            // chain bonus? then show it!
            
            if (g->bonusChainLength)
            {
                srcPixMap = GetGWorldPixMap(g->chainBonusGWorld);
                mskPixMap = GetGWorldPixMap(g->chainBonusMask);
                
                GetPixBounds(srcPixMap, &srcRect);
                dstRect = srcRect;
                OffsetRect(&dstRect, (620 - srcRect.right)/2, 23);
                
                LockPixels(srcPixMap);
                LockPixels(mskPixMap);
                
                CopyMask(   (BitMap *)*srcPixMap,
                            (BitMap *)*mskPixMap,
                            (BitMap *)*dstPixMap,
                            &srcRect,
                            &srcRect,
                            &dstRect);
                
                // dangling chains:
                
                srcRect.top += 6;
                dstRect.top += 6;
                
                i = 0;
                chainLevel = GetChainLevel();
                chainOffset = srcRect.bottom - 20;
                
                while (i < chainLevel)
                {
                    OffsetRect(&dstRect, 0, chainOffset);
                    
                    CopyMask(   (BitMap *)*srcPixMap,
                                (BitMap *)*mskPixMap,
                                (BitMap *)*dstPixMap,
                                &srcRect,
                                &srcRect,
                                &dstRect);
                    
                    i++;
                }
                
                UnlockPixels(srcPixMap);
                UnlockPixels(mskPixMap);
                
                // extra names for chain bonus
                
                i = 0;
                
                while (i <  chainLevel)
                {
                    srcPixMap = GetGWorldPixMap(g->extraBonusGWorld[5 - (chainLevel - i)]);
                    GetPixBounds(srcPixMap, &srcRect);
                    
                    dstRect = srcRect;
                    
                    OffsetRect(&dstRect, (620 - dstRect.right)/2, 72 + (chainOffset * i));
                    
                    LockPixels(srcPixMap);
                    
                    CopyBits(	(BitMap *)*srcPixMap,
                                (BitMap *)*dstPixMap,
                                &srcRect,
                                &dstRect,
                                srcCopy,
                                NULL);
                    
                    UnlockPixels(srcPixMap);
                    
                    i++;
                }
            }
            
            UnlockPixels(dstPixMap);
            
            SetGWorld(storePort, storeDevice);
        }
    }
}


void DrawLevelStartStuff(GWorldPtr theGWorld)
{
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap, dstPixMap, mskPixMap;
    Rect		srcRect, dstRect;
    
    GetGWorld(&storePort, &storeDevice);
    SetGWorld(theGWorld, NULL);
    
    srcPixMap = GetGWorldPixMap(g->msgLevelStartGWorld);
    mskPixMap = GetGWorldPixMap(g->msgLevelStartMask);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcRect);
    dstRect = srcRect;
    OffsetRect(&dstRect, (620 - srcRect.right)/2, 70);
    
    LockPixels(srcPixMap);
    LockPixels(mskPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(   (BitMap *)*srcPixMap,
                (BitMap *)*mskPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &srcRect,
                &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
    
    if ( (Button() && GetCurrentEventTime() - g->lastStateSwitchTime > 0.5 * kEventDurationSecond)
            || GetCurrentEventTime() - g->lastStateSwitchTime > 1.5 * kEventDurationSecond )
        StateSwitch(kPlaying);
    
}

void DrawLevelEndStuff(GWorldPtr theGWorld)
{
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap, dstPixMap, mskPixMap;
    Rect		srcRect, dstRect;
    
    GetGWorld(&storePort, &storeDevice);
    SetGWorld(theGWorld, NULL);
    
    srcPixMap = GetGWorldPixMap(g->msgLevelEndGWorld);
    mskPixMap = GetGWorldPixMap(g->msgLevelEndMask);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcRect);
    dstRect = srcRect;
    OffsetRect(&dstRect, (620 - srcRect.right)/2, 70);
    
    LockPixels(srcPixMap);
    LockPixels(mskPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(   (BitMap *)*srcPixMap,
                (BitMap *)*mskPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &srcRect,
                &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
    
    if ( (Button() && GetCurrentEventTime() - g->lastStateSwitchTime > 0.667 * kEventDurationSecond)
            || GetCurrentEventTime() - g->lastStateSwitchTime > 2 * kEventDurationSecond )
        NextLevel();
    
}


void DrawGameOverScreenStuff(GWorldPtr theGWorld)
{
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap, dstPixMap, mskPixMap;
    Rect		srcRect, dstRect;
    
    GetGWorld(&storePort, &storeDevice);
    SetGWorld(theGWorld, NULL);
    
    srcPixMap = GetGWorldPixMap(g->msgGameOverGWorld);
    mskPixMap = GetGWorldPixMap(g->msgGameOverMask);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcRect);
    dstRect = srcRect;
    OffsetRect(&dstRect, (620 - srcRect.right)/2, 70);
    
    LockPixels(srcPixMap);
    LockPixels(mskPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(   (BitMap *)*srcPixMap,
                (BitMap *)*mskPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &srcRect,
                &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
    
    if ( (Button() && GetCurrentEventTime() - g->lastStateSwitchTime > 2.2 * kEventDurationSecond)
            || GetCurrentEventTime() - g->lastStateSwitchTime > 10 * kEventDurationSecond )
        EndGame(TRUE);
    
    
    
}

void DrawCompletedScreenStuff(GWorldPtr theGWorld)
{
    
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap, dstPixMap, mskPixMap;
    Rect		srcRect, dstRect;
    
    GetGWorld(&storePort, &storeDevice);
    SetGWorld(theGWorld, NULL);
    
    srcPixMap = GetGWorldPixMap(g->msgCompletedGWorld);
    mskPixMap = GetGWorldPixMap(g->msgCompletedMask);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcRect);
    dstRect = srcRect;
    OffsetRect(&dstRect, (620 - srcRect.right)/2, 70);
    
    LockPixels(srcPixMap);
    LockPixels(mskPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(   (BitMap *)*srcPixMap,
                (BitMap *)*mskPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &srcRect,
                &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
    
    if ( (Button() && GetCurrentEventTime() - g->lastStateSwitchTime > 1.5 * kEventDurationSecond)
            || GetCurrentEventTime() - g->lastStateSwitchTime > 10 * kEventDurationSecond )
        EndGame(TRUE);
    
}


void DrawPausedStuff(GWorldPtr theGWorld)
{
    GWorldPtr		storePort;
    GDHandle		storeDevice;
    
    PixMapHandle	srcPixMap, dstPixMap, mskPixMap;
    Rect		srcRect, dstRect;
    
    GetGWorld(&storePort, &storeDevice);
    SetGWorld(theGWorld, NULL);
    
    srcPixMap = GetGWorldPixMap(g->msgPausedGWorld);
    mskPixMap = GetGWorldPixMap(g->msgPausedMask);
    dstPixMap = GetGWorldPixMap(theGWorld);
    
    GetPixBounds(srcPixMap, &srcRect);
    dstRect = srcRect;
    OffsetRect(&dstRect, (620 - srcRect.right)/2, 70);
    
    LockPixels(srcPixMap);
    LockPixels(mskPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(   (BitMap *)*srcPixMap,
                (BitMap *)*mskPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &srcRect,
                &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
    
}


void AddSheepArrow(short posx)
{
    if (g->sheepArrowsThisFrame < NUM_ARROWS)
    {
        g->sheepArrows[g->sheepArrowsThisFrame] = posx;
        
        g->sheepArrowsThisFrame += 1;
    }
    
}







