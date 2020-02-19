/*
 *  fire.c
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 17 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "fire.h"
#include "mafftypes.h"
#include "game.h"

extern GlobalStuff *g;

void ProcessAndDrawFire (GWorldPtr theGWorld)
{
    GDHandle			storeDevice;
    CGrafPtr			storePort;
    
    SheepToken			*thisSheep;
    PixMapHandle		brnPixMap;
    register unsigned char	*brn;
    unsigned long		brnRowBytes;
    short			lastFullByte;
    short			x, y, sizex, sizey, basex, basey;
    float			continuity, continuityComponentX, continuityComponentY;
    float			sheepSpeed;
    
    Point			thePoint;
    Rect			expandedFireBounds;
    short			value;
    unsigned long		offset;
    short			i,j;
    
    register unsigned char 	*fire;
    register long		*fireAsLongs;
    unsigned char		leftValue, storeValue;
    
    unsigned long		rowBytes;
    PixMapHandle		dstPixMap;
    register unsigned char	*dst;
    unsigned long		dstRowsBytes;
    
    bool			anySheepBurning = 0;
    char			randoms[256];
    char			sheepRandoms[256];
    short			randomCount = 0;
    
    while (randomCount <= 255)	// it may look silly, but it makes it faster.
    {
        randoms[randomCount] = MyRandom(-35, 5);
        randomCount++;
    }
    randomCount = 0;
    
    fire = g->fireArray;
    fireAsLongs = (long *)fire;
    thisSheep = g->baseSheep;
    GetGWorld(&storePort, &storeDevice);
    
    while (thisSheep) // add fire for each burning sheep
    {
        sizex = thisSheep->deadBounds.right;
        sizey = thisSheep->deadBounds.bottom;
        
        expandedFireBounds = g->fireBounds;
        InsetRect(&expandedFireBounds, -sizex/2, -sizey/2);
        
        thePoint.h = thisSheep->position.x;
        thePoint.v = thisSheep->position.y;
        
        if ( MyPtInRect(&thePoint, &expandedFireBounds) )
        {
                
            if (thisSheep->isBurning)
            {
                if (!anySheepBurning)
                {
                    anySheepBurning = 1;
                    
                    while (randomCount <= 255)	// set to 0 earlier
                    {
                        sheepRandoms[randomCount] = MyRandom(155,255);
                        randomCount++;
                    }
                    randomCount = 0;
                }
                
                brnPixMap = GetGWorldPixMap(thisSheep->burnMask);
                brnRowBytes = (**brnPixMap).rowBytes & 0x3FFF;
                LockPixels(brnPixMap);
                brn = GetPixBaseAddr(brnPixMap);
                
                if (brnRowBytes & 7)
                    lastFullByte = (brnRowBytes - 1) * 8 - 1;
                else
                    lastFullByte = sizex;
                
                SetGWorld(thisSheep->burnMask, NULL);
                
                sheepSpeed = sqrt(thisSheep->velocity.x*thisSheep->velocity.x + thisSheep->velocity.y*thisSheep->velocity.y);
                continuityComponentX = thisSheep->velocity.x/sheepSpeed;
                continuityComponentY = thisSheep->velocity.y/sheepSpeed;
                
                basex = thisSheep->position.x - sizex/2;
                basey = thisSheep->position.y - sizey/2;
                
                y = 0;
                
                while (y < sizey)
                {
                    x = 0;
                    
                    while (x < sizex)
                    {
                        if (brn[(brnRowBytes * y) + (x >> 3)] || x > lastFullByte)
                        {
                            
                            if (GetPixel(x,y))
                            {
                                continuity = 0;
                                
                                while (continuity < sheepSpeed)
                                {    
                                    thePoint.h = basex + x + continuityComponentX * continuity;
                                    thePoint.v = basey + y + continuityComponentY * continuity;
                                    
                                    if ( MyPtInRect(&thePoint, &g->fireBounds) )
                                    {
                                        fire[ (thePoint.v * 620) + thePoint.h ] = sheepRandoms[randomCount++];
                                        if (randomCount == 256) randomCount = 0;
                                    }
                                    
                                    continuity += 0.9;//1.6;
                                }
                            }
                            x++;
                        }
                        else
                        {
                            x += 8;
                        }
                        
                    }
                    y++;
                }
                UnlockPixels(brnPixMap);
                // don't set gworld back to original - we'll draw in another and set it back afterwards
            }
        }
        thisSheep = thisSheep->next;
    }
    
    // now do fire propagation + drawing
    
    
    
    dstPixMap = GetGWorldPixMap(theGWorld);
    rowBytes = (**dstPixMap).rowBytes & 0x3FFF;
    LockPixels(dstPixMap);
    
    dst = (unsigned char *)GetPixBaseAddr(dstPixMap);
    
    // already saved current gworld
    SetGWorld(theGWorld, NULL);
    
    offset = 1;
    j = 1;
    i = 0;
    leftValue = fire[0];
    dstRowsBytes = 0;
    
    while ( offset < (259780) ) // 620 * 419
    {
        if ( j == 619 ) // skip over edges
        {
            offset++;
            i++;
            j = 1;
            leftValue = fire[offset];
            offset++;
            
            dstRowsBytes = i * rowBytes;
        }
        
        if (fireAsLongs[offset >> 2] || j > 615)
        {
            storeValue = fire[offset];
            
            value = (-4 + leftValue + storeValue + fire[offset + 1] + fire[offset + 620]) >> 2;
            
            (value > 54) ? ( value += randoms[randomCount++] ) : (value = 0) ;
            
            if (randomCount == 256) randomCount = 0;
            
            fire[offset] = value;
            leftValue = storeValue;
            
            if (fire[offset] > 31)
            {
                if (fire[offset] > 100) // > 100 is fire, < 101 but nonzero is smoke
                {
                    dst[(j << 1) + dstRowsBytes] = FIRE_COLOUR_HI + ( fire[offset] >> 6 );
                    dst[(j << 1) + dstRowsBytes + 1] = FIRE_COLOUR_LO + ( (fire[offset] >> 3) << 6 );
                }
                else
                {
                    dst[(j << 1) + dstRowsBytes] = SMOKE_COLOUR_HI;
                    dst[(j << 1) + dstRowsBytes + 1] = SMOKE_COLOUR_LO;
                }
            }
            
            offset++;
            j++;
        }
        else
        {
            offset += 4;
            j += 4;
            leftValue = 0;
        }
        
    }
    
    UnlockPixels(dstPixMap);
    
    SetGWorld(storePort, storeDevice);
}

