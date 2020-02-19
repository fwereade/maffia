/*
 *  game.c
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 10 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

#include "game.h"
#include "graphics.h"
#include "fire.h"

extern void GetCompensatedMouse(Point *pt);


enum					// bonus chain level
{
    kBClevel1 = 3,			// 3
    kBClevel2 = 6,			// 6
    kBClevel3 = 12,			// 12
    kBClevel4 = 24,			// 24
    kBClevel5 = 36			// 48, ok then, 36.
};

extern GlobalStuff *g;
extern void CleanUp(bool instaQuit);
extern void HighScore(void);
extern void GameOver(void);
extern void ReStartTimer(void);
extern bool NewHighScore(void);
extern void Instructions(void);
extern void AddSheepArrow(short posx);

extern void PlaySound(short channel, Handle sound);

void NewGame (short level)
{
    short	i = 1;
    
    RemoveAllSheep();
    RemoveAllScoreEffects();
    RemoveAllShotEffects();
    EmptyFireBuffer();
    
    g->theScoreStuff.score = 0;
    g->theScoreStuff.highestMultiplier = 0;
    g->theScoreStuff.longestJuggle = 0;
    g->theScoreStuff.sheepKilledTotal = 0;
    g->theScoreStuff.sheepUsedThisLevel = 0;
    g->theScoreStuff.livesLeft = LIVES_START;
    
    g->theLevel = g->baseLevel;
    while (i < level)
    {
        g->theLevel = g->theLevel->next;
        i++;
    }
    
    g->theWeapon = g->baseWeapon;
    
    g->lastBonus = kNoBonus;
    g->bonusChainLength = 0;
    g->inGame = 1;
    
    StateSwitch(kLevelStart);
}

void NextLevel(void)
{
    RemoveAllSheep();
    RemoveAllScoreEffects();
    RemoveAllShotEffects();
    EmptyFireBuffer();
    
    g->theScoreStuff.sheepUsedThisLevel = 0;
    
    if (g->theLevel && g->theLevel->next)
    {
        g->theLevel = g->theLevel->next;
    	StateSwitch(kLevelStart);
    }
    else
        EndGame(TRUE);
    
    g->lastBonus = kNoBonus;
    g->bonusChainLength = 0;
}

void EndGame(bool showNonHighScore)
{
    g->inGame = 0;
    
    DrawInterface(g->swapGWorld);
    DrawGWorldToWindow(g->swapGWorld, g->theWindow);
    
    if (!(g->theScoreStuff.score))
        return;
    
    if (NewHighScore())
        HighScore();
    else
    {
        if (showNonHighScore)
            GameOver();
    }
}

void OneFrame(void)
{
    Point	clickPoint;
    GWorldPtr	currentPort;
    GDHandle	currentDevice;
    
    if (g->autoPauseWindow == FrontNonFloatingWindow() && !g->alreadyAutoPaused)
        return;
    
    g->frameTime = GetCurrentEventTime();
    g->bonusAlreadyThisFrame = FALSE;
    
    CheckKeys();
    ProcessSheep();
    
    if (!g->inGame)
    {
        RemoveAllSheep();
        DrawInterface(g->swapGWorld);
        return;
    }
    
    ProcessScoreEffects();
    ProcessShotEffects();
    RemoveDeadStuff();
    
    if ( (g->frameTime - g->theWeapon->lastFired) > g->theWeapon->reloadTime * 30 / g->fps ) // turbo mode hack
    {
        GetGWorld(&currentPort, &currentDevice);
        
        if (Button() && GetWindowPort(g->theWindow) == currentPort)
        {
            GetCompensatedMouse(&clickPoint);
            if (MyPtInRect(&clickPoint, &g->swapBounds))
                FireWeapon(clickPoint);
            g->theWeapon->lastFired = g->frameTime;
        }
    }
    
    DrawBackground(g->swapGWorld);
    DrawStuff(g->swapGWorld);		// that is, scenery and sheep
    
    if (g->pref.fire == kOn)
        ProcessAndDrawFire(g->swapGWorld);
    
    DrawScoreEffects(g->swapGWorld);
    DrawShotEffects(g->swapGWorld);
    
    DrawScore(g->swapGWorld);
    DrawBonus(g->swapGWorld);
}



void EmptyFireBuffer (void)
{
    long i = 0;
    
    while (i < 260400) // 620 * 420
    {
        g->fireArray[i] = 0;
        i++;
    }
}

float MyRandom (float min, float max)
{
    float range;
    unsigned short random;
    
    range = max-min;
    random = Random();
    
    return (random*range/65535) + min;
}

short MyPtInRect (Point *pt, Rect *rect)
{
     return pt->h >= rect->left && pt->h < rect->right && 
            pt->v >= rect->top && pt->v < rect->bottom;
}

void NewRandomSheep (void)
{
    short		startCounter,
                        numStartPoints,
                        startPointChosen,
                        left;
    
    StartPoint		*thisPoint;
    
    thisPoint = g->theLevel->baseStartPoint;
    numStartPoints = g->theLevel->numStartPoints;
    startPointChosen = MyRandom(0, numStartPoints);
    startCounter = 0;
    
    if (thisPoint)
    {
        while (startCounter < startPointChosen && thisPoint->next)
        {
            thisPoint = thisPoint->next;
            startCounter++;
        }
        
        left = (short)MyRandom(0,1.99);
        
        if (left)
            NewSheep(thisPoint->layer, -50, thisPoint->posY, MyRandom(g->theLevel->minSpeed, g->theLevel->maxSpeed), 0);
        else
            NewSheep(thisPoint->layer, 670, thisPoint->posY, -(MyRandom(g->theLevel->minSpeed, g->theLevel->maxSpeed)), 0);
    }
    
}



void NewSheep (short layer, float posx, float posy, float velx, float vely)
{
    OSErr		err;
    SheepToken		*newSheep;
    SheepToken		*thisSheep;
    Rect		theRect;
    
    g->theScoreStuff.sheepUsedThisLevel += 1;
    
    newSheep = (SheepToken *)NewPtr(sizeof(SheepToken));
    
    newSheep->readyToDie = false;
    newSheep->layer = layer;
    newSheep->position.x = posx;
    newSheep->position.y = posy;
    newSheep->velocity.x = velx;
    newSheep->velocity.y = vely;
    newSheep->deadBounds = g->theSheepType.deadBounds;
    newSheep->timesShot = 0;
    newSheep->isBurning = false;
    
    newSheep->frame = 1;
    newSheep->lastFrameTime = g->frameTime;
    
    theRect = g->theSheepType.deadBounds;
    
    err = NewGWorld(&newSheep->deadSprite,
                    16,
                    &theRect,
                    NULL,
                    NULL,
                    0);
    if (err) CleanUp(TRUE);
    
    err = NewGWorld(&newSheep->deadSpriteMaskWithoutOutline,
                    1,
                    &theRect,
                    NULL,
                    NULL,
                    0);
    if (err) CleanUp(TRUE);
    
    err = NewGWorld(&newSheep->deadSpriteMaskWithOutline,
                    1,
                    &theRect,
                    NULL,
                    NULL,
                    0);
    if (err) CleanUp(TRUE);
    
    err = NewGWorld(&newSheep->burnMask,
                    1,
                    &theRect,
                    NULL,
                    NULL,
                    0);
    if (err) CleanUp(TRUE);
    ClearGWorld(newSheep->burnMask, whiteColor);
    
    
    if (newSheep->velocity.x >= 0)
    {
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteRight, &newSheep->deadSprite);
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteRightMaskWithoutOutline, &newSheep->deadSpriteMaskWithoutOutline);
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteRightMaskWithOutline, &newSheep->deadSpriteMaskWithOutline);
    }
    else
    {
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteLeft, &newSheep->deadSprite);
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteLeftMaskWithoutOutline, &newSheep->deadSpriteMaskWithoutOutline);
        InitSheepGWorld(&g->theSheepType.originalDeadSpriteLeftMaskWithOutline, &newSheep->deadSpriteMaskWithOutline);
    }
    
    if (g->baseSheep)
    {
        thisSheep = g->baseSheep;
        
        while (thisSheep->next)
        {
            thisSheep = thisSheep->next;
        }
        
        newSheep->prev = thisSheep;
        newSheep->next = NULL;
        thisSheep->next = newSheep;
        
    }
    else
    {
        newSheep->prev = NULL;
        newSheep->next = NULL;
        g->baseSheep = newSheep;
    }
    
}



void RemoveSheep (SheepToken *theSheep)
{
    SheepToken		*thisSheep;
    
    if (theSheep != g->baseSheep)
    {
        thisSheep = g->baseSheep;
        while (thisSheep->next != theSheep)
        {
            thisSheep = thisSheep->next;
        }
        thisSheep->next = theSheep->next;
        if (theSheep->next)
            theSheep->next->prev = thisSheep;
    }
    else
    {
        g->baseSheep = theSheep->next;
        if (g->baseSheep)
            g->baseSheep->prev = NULL;
    }
    
    DisposeGWorld(theSheep->deadSprite);
    DisposeGWorld(theSheep->deadSpriteMaskWithoutOutline);
    DisposeGWorld(theSheep->deadSpriteMaskWithOutline);
    DisposeGWorld(theSheep->burnMask);
    
    DisposePtr((Ptr)theSheep);
}

void RemoveAllSheep (void)
{
    while (g->baseSheep)
    {
        RemoveSheep(g->baseSheep);
    }
    g->baseSheep = NULL;
}

void RemoveWeapon (Weapon *theWeapon)
{
    Weapon	*thisWeapon;
    
    if (theWeapon != g->baseWeapon)
    {
            thisWeapon = g->baseWeapon;
            
            while (thisWeapon->next != theWeapon)
            {
                    thisWeapon = thisWeapon->next;
            }
            
            thisWeapon->next = theWeapon->next;
    }else{
            g->baseWeapon = theWeapon->next;
    }
    
    DisposeGWorld(theWeapon->holeSprite);
    DisposeGWorld(theWeapon->holeSpriteMaskWithoutOutline);
    DisposeGWorld(theWeapon->holeSpriteMaskWithOutline);
    DisposeGWorld(theWeapon->holeSpriteDrawMask);
    DisposeGWorld(theWeapon->holeTempMask);
    
    DisposePtr((Ptr)theWeapon);
}

void RemoveAllWeapons (void)
{
    while (g->baseWeapon)
    {
        RemoveWeapon(g->baseWeapon);
    }
    
    g->baseWeapon = NULL;
}

void AddScoreEffect (short score, short multiplier, Point position)
{
    ScoreEffect		*newScoreEffect;
    ScoreEffect		*thisScoreEffect;
    short			counter;
    
    newScoreEffect = (ScoreEffect *)NewPtr(sizeof(ScoreEffect));
    
    newScoreEffect->readyToDie = false;
    newScoreEffect->score = score;
    newScoreEffect->multiplier = multiplier;
    
    counter = 0;
    while (score >= pow(10, counter))
    {
        counter++;
    }
    newScoreEffect->scoreCharacters = counter;
    if (multiplier > 1)
    {
        counter = 0;
        while (multiplier >= pow(10, counter))
        {
            counter++;
        }
        newScoreEffect->multiplierCharacters = counter;
        newScoreEffect->totalCharacters = newScoreEffect->scoreCharacters + newScoreEffect->multiplierCharacters + 1;
    }
    else
    {
        newScoreEffect->multiplierCharacters = 0;
        newScoreEffect->totalCharacters = newScoreEffect->scoreCharacters;
    }
    
    newScoreEffect->animationFrame = -1;
    newScoreEffect->position = position;
    
    if (g->baseScoreEffect)
    {
        thisScoreEffect = g->baseScoreEffect;
        
        while(thisScoreEffect->next)
        {
            thisScoreEffect = thisScoreEffect->next;
        }
        
        thisScoreEffect->next = newScoreEffect;
    }
    else
    {
        g->baseScoreEffect = newScoreEffect;
    }
    
    newScoreEffect->next = NULL;
}
		
void RemoveScoreEffect (ScoreEffect *theScoreEffect)
{
    ScoreEffect		*thisScoreEffect;
    
    if (theScoreEffect != g->baseScoreEffect)
    {
        thisScoreEffect = g->baseScoreEffect;
        
        while (thisScoreEffect->next != theScoreEffect)
        {
                thisScoreEffect = thisScoreEffect->next;
        }
        
        thisScoreEffect->next = theScoreEffect->next;
    }
    else
    {
        g->baseScoreEffect = theScoreEffect->next;
    }
    
    DisposePtr((Ptr)theScoreEffect);
}

void RemoveAllScoreEffects (void)
{
    while (g->baseScoreEffect)
    {
        RemoveScoreEffect(g->baseScoreEffect);
    }
    g->baseScoreEffect = NULL;
}

void AddShotEffect (Point where, short type)
{
    ShotEffect		*newShot, *theShot;
    
    if (!(g->pref.shotFX))
        return;
    
    newShot = (ShotEffect *)NewPtr(sizeof(ShotEffect));
    if (!newShot)
        CleanUp(TRUE);
    
    newShot->readyToDie = FALSE;
    newShot->type = type;
    newShot->position = where;
    newShot->animationFrame = 0;
    newShot->next = NULL;
    
    theShot = g->baseShotEffect;
    
    if (!theShot)
        g->baseShotEffect = newShot;
    else
    {
        while (theShot->next)
            theShot = theShot->next;
        
        theShot->next = newShot;
    }
    
}

void RemoveShotEffect (ShotEffect *theShotEffect)
{
    ShotEffect		*thisShotEffect;
    
    if (theShotEffect != g->baseShotEffect)
    {
        thisShotEffect = g->baseShotEffect;
        
        while (thisShotEffect->next != theShotEffect)
        {
                thisShotEffect = thisShotEffect->next;
        }
        
        thisShotEffect->next = theShotEffect->next;
    }
    else
    {
        g->baseShotEffect = theShotEffect->next;
    }
    
    DisposePtr((Ptr)theShotEffect);
}

void RemoveAllShotEffects (void)
{
    while (g->baseShotEffect)
    {
        RemoveShotEffect(g->baseShotEffect);
    }
    g->baseShotEffect = NULL;
}




void ProcessSheep (void)
{
    SheepToken			*thisSheep;
    short			counter = 0;
    static EventTime		lastReleaseTime = 0;
    
    if (g->frameTime - lastReleaseTime > MyRandom(g->theLevel->minDelay, g->theLevel->maxDelay))
    {
        if (g->theLevel->numSheep > g->theScoreStuff.sheepUsedThisLevel)
        {
            NewRandomSheep();
            lastReleaseTime = g->frameTime;
        }
    }
    
    
    if (g->baseSheep)
    {
        thisSheep = g->baseSheep;
        
        while (thisSheep)
        {
            if ( -((float)kGravity/2) < thisSheep->velocity.y
                && thisSheep->velocity.y <= ((float)kGravity/2)
                && thisSheep->position.y < -420)
                HighFlierBonus(-420 - thisSheep->position.y);
            
            if (thisSheep->position.y >= 460)
            {
                thisSheep->readyToDie = true;
            }
            else if (thisSheep->position.y <= -40)
            {
                AddSheepArrow((short)thisSheep->position.x);
            }
            
            if (thisSheep->timesShot)
            {
                counter++;
                
                thisSheep->velocity.y += (float)kGravity;
                
                if (thisSheep->position.x > 680 || thisSheep->position.x < -60)
                    thisSheep->readyToDie = true;
            }
            else
            {
                if (thisSheep->position.x > 680 || thisSheep->position.x < -60)
                {
                    PlaySound(kBaaChannel, g->sounds.baa);
                    
                    thisSheep->readyToDie = true;
                    
                    g->theScoreStuff.livesLeft--;
                    
                    BreakBonusChain();
                    
                    if (!g->theScoreStuff.livesLeft)
                    {
                        StateSwitch(kGameOverScreen);
                    }
                    
                }
            }
            
            if ( g->frameTime - thisSheep->lastFrameTime > kSheepAnimWait/fabs(thisSheep->velocity.x) )
            {
                if (thisSheep->frame)
                {
                    thisSheep->frame = 0;
                    thisSheep->lastFrameTime = g->frameTime;
                }else{
                    thisSheep->frame = 1;
                    thisSheep->lastFrameTime = g->frameTime;
                }
            }
            
            thisSheep->position.x += thisSheep->velocity.x;
            thisSheep->position.y += thisSheep->velocity.y;
            
            thisSheep = thisSheep->next;
        }
    }
    else
    {
        if (g->theLevel->numSheep > g->theScoreStuff.sheepUsedThisLevel)
        {
            NewRandomSheep();
            lastReleaseTime = g->frameTime;
        }
        else if (g->gameState != kGameOverScreen)
        {
            //////////////////////////////////////////////////////////////////go to next level
            if (g->theLevel->next)
                StateSwitch(kLevelEnd);
            else
                StateSwitch(kCompletedScreen);
        }
    }
    
    if (counter)
        g->theScoreStuff.multiplier = counter;
    else
        g->theScoreStuff.multiplier = 1;
    
    if (g->theScoreStuff.multiplier > g->theScoreStuff.highestMultiplier)
    {
        g->theScoreStuff.highestMultiplier = g->theScoreStuff.multiplier;
    }
	
}





void ProcessScoreEffects (void)
{
    ScoreEffect		*theScoreEffect;
    
    theScoreEffect = g->baseScoreEffect;
    
    while (theScoreEffect)
    {
        theScoreEffect->animationFrame++;
        
        if (theScoreEffect->animationFrame > 45)
        {
            theScoreEffect->readyToDie = true;
        }
        theScoreEffect = theScoreEffect->next;
    }
}

void ProcessShotEffects (void)
{
    ShotEffect		*theShotEffect;
    
    theShotEffect = g->baseShotEffect;
    
    while (theShotEffect)
    {
        theShotEffect->animationFrame++;
        
        if (theShotEffect->animationFrame > 19)
        {
            theShotEffect->readyToDie = true;
        }
        theShotEffect = theShotEffect->next;
    }
}

void CheckKeys(void)
{
    KeyMap	theKeys;
    Weapon	*theWeapon = NULL;
    
    GetKeys(theKeys);
    
    if (((unsigned char *)(theKeys))[(0x35) / 8] & 1 << ((0x35) % 8))	// escape key
        Instructions();
    
    if (g->windowActive && g->inGame && g->gameState <= kPlaying && !(alphaLock & GetCurrentKeyModifiers()) ) // potentially nasty: <= kPlaying
    {
        if ((theKeys[0] & 1 << 10) || (((unsigned char *)(theKeys))[(0x53) / 8] & 1 << ((0x53) % 8)))	// either '1' key
            theWeapon = g->baseWeapon;		
        
        if ((theKeys[0] & 1 << 11) || (((unsigned char *)(theKeys))[(0x54) / 8] & 1 << ((0x54) % 8)))	// either '2' key
            theWeapon = g->baseWeapon->next;
        
        if ((theKeys[0] & 1 << 12) || (((unsigned char *)(theKeys))[(0x55) / 8] & 1 << ((0x55) % 8)))	// either '3' key
            theWeapon = g->baseWeapon->next->next;
        
        if ((theKeys[0] & 1 << 13) || (((unsigned char *)(theKeys))[(0x56) / 8] & 1 << ((0x56) % 8)))	// either '4' key
            theWeapon = g->baseWeapon->next->next->next;
    }
    
    if (theWeapon && theWeapon != g->theWeapon)
    {
        g->theWeapon = theWeapon;
        PlaySound(kBleepChannel, g->sounds.weaponSwitch);
    }
}

void FireWeapon (Point thePoint)
{
    SheepToken		*theSheep = NULL;
    SceneryToken		*theScenery = NULL;
    Rect			collisionRect, sizeRect;
    PixMapHandle		collisionMask;
    GWorldPtr		maskGWorld;
    Point			collisionPoint;
    unsigned long		rowBytes;
    short			offset;
    Ptr			maskPixBaseAddress;
    short			layer; // if a sheeep and some scenery are on the same layer, the sheep is in front
    short			numHit = 0, hitScenery = 0;
    
    SetCursor(*g->crosshair);
    
    if (g->gameState != kPlaying)
        return;
    
    PlaySound(kShotChannel, g->theWeapon->shotSoundHandle);
    
    //	first, test scenery objects and find what layers
    //	are not behind scenery at this point- only sheep 
    //	on these layers will be tested. 
    
    layer = 20; // bigger than it will ever be
    
    if (g->theLevel->baseSceneryToken)
        theScenery = g->theLevel->baseSceneryToken;
    
    
    while (theScenery)
    {
        if (theScenery->layer < layer)
        {
            sizeRect = theScenery->type->bounds;
            maskGWorld = theScenery->type->maskGWorld;
            collisionMask = GetGWorldPixMap(maskGWorld);
            
            collisionRect.left = (short)theScenery->position.x - sizeRect.right/2;
            collisionRect.right = (short)theScenery->position.x + sizeRect.right/2;
            collisionRect.top = (short)theScenery->position.y - sizeRect.bottom/2;
            collisionRect.bottom = (short)theScenery->position.y + sizeRect.bottom/2;
            
            if (sizeRect.right & 1) collisionRect.right++;
            if (sizeRect.bottom & 1) collisionRect.bottom++;
            
            if ( MyPtInRect(&thePoint, &collisionRect) )
            {
                collisionPoint.h = thePoint.h - collisionRect.left;
                collisionPoint.v = thePoint.v - collisionRect.top;
                
                if (collisionMask)
                {
                    LockPixels(collisionMask);
                    rowBytes = (**collisionMask).rowBytes & 0x3FFF;
                    maskPixBaseAddress = GetPixBaseAddr(collisionMask);
                    
                    offset = collisionPoint.h / 8 + collisionPoint.v * rowBytes;
                    
                    if ( maskPixBaseAddress[offset] & ( 128 >> (collisionPoint.h % 8) ) )
                    {
                        layer = theScenery->layer;
                        hitScenery = 1;
                    }
                    
                    UnlockPixels(collisionMask);
                }
            }
        }
        theScenery = theScenery->next;
    }
    
    // now test for hits only those sheep that are possibly hittable
    
    if (g->baseSheep)
    {
            theSheep = g->baseSheep;
    }
    
    while (theSheep)
    {
        if (theSheep->layer <= layer)
        {
            if (theSheep->timesShot)
            {
                sizeRect = theSheep->deadBounds;
                maskGWorld = theSheep->deadSpriteMaskWithOutline;
                collisionMask = GetGWorldPixMap(maskGWorld);
            }
            else
            {
                
                if (theSheep->frame == 1)
                {
                    if (theSheep->velocity.x <= 0)
                        maskGWorld = g->theSheepType.liveSpriteRunLeftMaskA;
                    else
                        maskGWorld = g->theSheepType.liveSpriteRunRightMaskA;
                }
                else
                {
                    if (theSheep->velocity.x <= 0)
                        maskGWorld = g->theSheepType.liveSpriteRunLeftMaskB;
                    else
                        maskGWorld = g->theSheepType.liveSpriteRunRightMaskB;
                }
                
                collisionMask = GetGWorldPixMap(maskGWorld);
                sizeRect = (**collisionMask).bounds;
            }
            
            collisionRect.left = (short)theSheep->position.x - sizeRect.right/2;
            collisionRect.right = (short)theSheep->position.x + sizeRect.right/2;
            collisionRect.top = (short)theSheep->position.y - sizeRect.bottom/2;
            collisionRect.bottom = (short)theSheep->position.y + sizeRect.bottom/2;
            
            if (sizeRect.right & 1) collisionRect.right++;
            if (sizeRect.bottom & 1) collisionRect.bottom++;
            
            if ( MyPtInRect(&thePoint, &collisionRect) )
            {
                collisionPoint.h = thePoint.h - collisionRect.left;
                collisionPoint.v = thePoint.v - collisionRect.top;
                
                if (collisionMask)
                {
                    LockPixels(collisionMask);
                    rowBytes = (**collisionMask).rowBytes & 0x3FFF;
                    maskPixBaseAddress = GetPixBaseAddr(collisionMask);
                    
                    offset = collisionPoint.h / 8 + collisionPoint.v * rowBytes;
                    
                    if ( maskPixBaseAddress[offset] & ( 128 >> (collisionPoint.h % 8) ) )
                    {
                            ShootSheep(theSheep, collisionPoint);
                            numHit++;
                    }
                    
                    UnlockPixels(collisionMask);
                }
            }
                
        }
        theSheep = theSheep->next;
    }
    
    if (numHit)
    {
        PlaySound(kSplatChannel, g->sounds.splat);
    
        if (numHit > 1)
            ShishKebabBonus(numHit);
        
        AddShotEffect(thePoint, kHitSheep);
    }
    else
    {
        if (hitScenery)
            AddShotEffect(thePoint, kHitScenery);
        else
            AddShotEffect(thePoint, kHitNowt);
    }
}

void ShootSheep(SheepToken *theSheep, Point hitPoint)
{
    PixMapHandle	srcPixMap, mskPixMap, dstPixMap;
    Rect		srcRect, mskRect, dstRect, sizeRect;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    
    short			bulletSprite;    // 0 to 7
    float			sideRecoil;
    Point			windowLocation;
    short			score;
    short			distanceOffscreen;
    
    
    GetGWorld(&storePort, &storeDevice);
    sizeRect = theSheep->deadBounds;
    windowLocation.h = hitPoint.h - sizeRect.right/2 + theSheep->position.x;
    windowLocation.v = hitPoint.v - sizeRect.bottom/2 + theSheep->position.y;
    
    // cut the hole with outline out of sheep mask without outline
    
    SetGWorld(theSheep->deadSpriteMaskWithoutOutline, NULL);
    srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithOutline);
    dstPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithoutOutline);
    
    srcRect = g->theWeapon->holeRect;
    
    dstRect.left = hitPoint.h - srcRect.right/2;
    dstRect.right = hitPoint.h + srcRect.right/2;
    
    if (srcRect.right & 1) dstRect.right++;
    
    bulletSprite = MyRandom(0.0, 7.99);
    srcRect.left += (srcRect.bottom * bulletSprite);
    srcRect.right += (srcRect.bottom * bulletSprite);
    
    if (theSheep->timesShot)
    {
            dstRect.top = hitPoint.v - srcRect.bottom/2;
            dstRect.bottom = hitPoint.v + srcRect.bottom/2;
    }else{
            dstRect.top = sizeRect.bottom - hitPoint.v - srcRect.bottom/2;
            dstRect.bottom = sizeRect.bottom - hitPoint.v + srcRect.bottom/2;
    }
            
    if (srcRect.bottom & 1) dstRect.bottom++;
    
    // debug stuff
    
//	if ( (srcRect.right - srcRect.left) != (dstRect.right - dstRect.left) ) SysBeep (1);
//	if ( (srcRect.bottom - srcRect.top) != (dstRect.bottom - dstRect.top) ) SysBeep (1);
    
    // end debug stuff
    
    LockPixels(srcPixMap);
    LockPixels(dstPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &dstRect,
                srcBic,
                NULL);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(dstPixMap);
    
    // cut the hole without outline out of [sheep mask with outline] and [burnmask]
    
    SetGWorld(theSheep->deadSpriteMaskWithOutline, NULL);
    srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithoutOutline);
    dstPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
    
    LockPixels(srcPixMap);
    LockPixels(dstPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &dstRect,
                srcBic,
                NULL);
    
    UnlockPixels(dstPixMap);
    
    SetGWorld(theSheep->burnMask, NULL);
    dstPixMap = GetGWorldPixMap(theSheep->burnMask);
    
    LockPixels(dstPixMap);
    
    CopyBits(	(BitMap *)*srcPixMap,
                (BitMap *)*dstPixMap,
                &srcRect,
                &dstRect,
                srcBic,
                NULL);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(dstPixMap);
    
    
    
    // make mask for bullet hole//////////////////////////////////////////////////////////////////////////
    
    if (g->pref.blood == kOn || g->theWeapon->weaponID == kFlamer) // flamer always leaves scorch marks
    {
        SetGWorld(g->theWeapon->holeTempMask, NULL);
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteDrawMask);
        mskPixMap = GetGWorldPixMap(g->theWeapon->holeTempMask);
        
        mskRect = g->theWeapon->holeRect;
        
        LockPixels(srcPixMap);
        LockPixels(mskPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    &srcRect,
                    &mskRect,
                    srcCopy,
                    NULL);
        
        UnlockPixels(srcPixMap);
        srcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithoutOutline);
        LockPixels(srcPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    &dstRect,
                    &mskRect,
                    notSrcBic,
                    NULL);
        
        UnlockPixels(srcPixMap);
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithOutline);
        LockPixels(srcPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    &srcRect,
                    &mskRect,
                    srcOr,
                    NULL);
        
        UnlockPixels(srcPixMap);
    }
    else // blood pref is off, and we aren't using a flamer
    {
        // we want a mask that won't draw any blood - we copy holeWithOutline to holeTempMask
        mskRect = g->theWeapon->holeRect;
        
        SetGWorld(g->theWeapon->holeTempMask, NULL);
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithOutline);
        mskPixMap = GetGWorldPixMap(g->theWeapon->holeTempMask);
        
        LockPixels(srcPixMap);
        LockPixels(mskPixMap);
        
        CopyBits(	(BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    &srcRect,
                    &mskRect,
                    srcCopy,
                    NULL);
        
        UnlockPixels(srcPixMap);
    }
    
    // put the (bloody-or-not) bullet hole onto the sheep
    
    SetGWorld(theSheep->deadSprite, NULL);
    srcPixMap = GetGWorldPixMap(g->theWeapon->holeSprite);
    dstPixMap = GetGWorldPixMap(theSheep->deadSprite);
    
    LockPixels(srcPixMap);
    LockPixels(dstPixMap);
    
    CopyMask(	(BitMap *)*srcPixMap,
                    (BitMap *)*mskPixMap,
                    (BitMap *)*dstPixMap,
                    &srcRect,
                    &mskRect,
                    &dstRect);
    
    UnlockPixels(srcPixMap);
    UnlockPixels(mskPixMap);
    UnlockPixels(dstPixMap);
    
    
    if (g->theWeapon->weaponID == kFlamer)  
    {
        theSheep->isBurning = true;
        
        // put the new bits of burn into holetempmask
        
        SetGWorld(g->theWeapon->holeTempMask, NULL);
        
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithOutline);
        dstPixMap = GetGWorldPixMap(g->theWeapon->holeTempMask);
        
        LockPixels(srcPixMap);
        LockPixels(dstPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &srcRect,
                    &mskRect,
                    srcCopy,
                    NULL);
        
        UnlockPixels(srcPixMap);
        srcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
        LockPixels(srcPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &dstRect,
                    &mskRect,
                    notSrcBic,
                    NULL);
        
        UnlockPixels(srcPixMap);
        UnlockPixels(dstPixMap);
        
        // copy that lot into burnmask
        
        SetGWorld(theSheep->burnMask, NULL);
        
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeTempMask);
        dstPixMap = GetGWorldPixMap(theSheep->burnMask);
        
        LockPixels(srcPixMap);
        LockPixels(dstPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &mskRect,
                    &dstRect,
                    srcOr,
                    NULL);
        
        // cut hole w/o outline out of burnmask
        
        UnlockPixels(srcPixMap);
        srcPixMap = GetGWorldPixMap(g->theWeapon->holeSpriteMaskWithoutOutline);
        LockPixels(srcPixMap);
        
        CopyBits(   (BitMap *)*srcPixMap,
                    (BitMap *)*dstPixMap,
                    &srcRect,
                    &dstRect,
                    srcBic,
                    NULL);
        
        UnlockPixels(srcPixMap);
        UnlockPixels(dstPixMap); // end of flamer "if"
    }
    
    SetGWorld(storePort, storeDevice);
    
    // end of drawing new sheep graphics
    
    // game effects - bonuses and high score
    
    theSheep->timesShot++;
    
    if (theSheep->timesShot > g->theScoreStuff.longestJuggle)
        g->theScoreStuff.longestJuggle = theSheep->timesShot;
    
    if (theSheep->timesShot == 1) // first hit
    {
        g->theScoreStuff.sheepKilledTotal++;
        theSheep->layer = 0; // remember, lower numbers are in front.
        
        if (theSheep->velocity.x < 0)
            theSheep->velocity.x = -(sqrt(-theSheep->velocity.x));
        else
            theSheep->velocity.x = sqrt(theSheep->velocity.x);
        
        // check for Jammy Dodger bonus - ie sheep is nearly offscreen on first shot
        
        distanceOffscreen = 0;
        
        if (theSheep->position.x > 620 && theSheep->velocity.x > 0)
            distanceOffscreen = (short)(theSheep->position.x) - 620;
        else if (theSheep->position.x < 0 && theSheep->velocity.x < 0)
            distanceOffscreen = -(short)(theSheep->position.x);
        
        if (distanceOffscreen)
            JammyDodgerBonus(distanceOffscreen);
    }
    
    if ( !(theSheep->timesShot % 25) )
        SwissCheeseBonus(theSheep->timesShot / 25);
    
    // actual score (as shown floating on screen)
    
    score = theSheep->timesShot * g->theWeapon->scoreMultiplier;
    
    if (g->pref.turbo)
        score *= 2;
    
    // don't multiply by chunk multiplier yet, so we can keep the 2 separate for the ScoreEffect
    
    AddScoreEffect(score, g->theScoreStuff.multiplier, windowLocation);
    
    g->theScoreStuff.score += score * g->theScoreStuff.multiplier;
    
    // game effects - physics
    
    theSheep->velocity.y -= g->theWeapon->recoil;
    sideRecoil = (sizeRect.right - 2 * hitPoint.h) * g->theWeapon->recoil;
    theSheep->velocity.x += sideRecoil / (4 * sizeRect.right);
    
    SplitSheep(theSheep);
}

void SplitSheep (SheepToken *theSheep)
{
    Rect		firstSheepBounds;
    Boolean		doneCopying = false;
    Rect		chunkSrcRect, chunkDstRect;
    PixMapHandle	srcPixMap, mskPixMap;
    GDHandle		storeDevice;
    CGrafPtr		storePort;
    OSErr		err;
    short		seedh, seedv;
    Point		thePoint;
    unsigned long	mskRowBytes, srcRowBytes;
    unsigned long	offset;
    Ptr			mskPixBaseAddr, srcPixBaseAddr;
    Boolean		foundBlack;
    SheepToken		*newSheep, *thisSheep;
    pointFloat		burstVector;
    short		numPieces = 0;
    
    PixMapHandle	copSrcPixMap, copDstPixMap;
    
    //
    // note - big ugly slob of a function, and very probably seriously inefficient.
    //
    //
    //
    //
    
    firstSheepBounds = theSheep->deadBounds;
    
    GetGWorld(&storePort, &storeDevice);
    
    // loop until maskwith is empty
    while (!doneCopying)
    {
        // find first black pixel in maskwith
        
        foundBlack = false;
        
        srcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
        LockPixels(srcPixMap);
        srcRowBytes = (**srcPixMap).rowBytes & 0x3FFF;
        srcPixBaseAddr = GetPixBaseAddr(srcPixMap);
        
        seedv = 0;
        
        while (seedv < firstSheepBounds.bottom && !foundBlack)
        {
            seedh = 0;
            
            while (seedh < firstSheepBounds.right && !foundBlack)
            {
                offset = (seedh >> 3) + (seedv * srcRowBytes);
                
                if ( srcPixBaseAddr[offset] & ( 128 >> (seedh & 7) ) ) // [seedh & 7] == [seedh % 8]
                {
                    foundBlack = true;
                    thePoint.h = seedh;
                    thePoint.v = seedv;
                }
                seedh++;
            }
            seedv++;
        }
        
        UnlockPixels(srcPixMap);
        
        if (foundBlack)
        {
            numPieces++;
            
            ClearGWorld(g->theSheepType.splitMask, whiteColor);
            
            chunkSrcRect.left = thePoint.h;
            chunkSrcRect.right = thePoint.h + 1;
            chunkSrcRect.top = thePoint.v;
            chunkSrcRect.bottom = thePoint.v + 1;
            
            srcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
            mskPixMap = GetGWorldPixMap(g->theSheepType.splitMask);
            
            LockPixels(srcPixMap);
            LockPixels(mskPixMap);
            
            srcRowBytes = (**srcPixMap).rowBytes & 0x3FFF;
            mskRowBytes = (**mskPixMap).rowBytes & 0x3FFF;
            
            srcPixBaseAddr = GetPixBaseAddr(srcPixMap);
            mskPixBaseAddr = GetPixBaseAddr(mskPixMap);
            
            // mask off the chunk
            
            FloodFill(	srcPixBaseAddr,
                        mskPixBaseAddr,
                        srcRowBytes,
                        mskRowBytes,
                        thePoint,
                        &firstSheepBounds,
                        &chunkSrcRect);
            
            // bit of explanation - it fills msk with all contiguous black pixels from src, starting frm thePoint
            
            UnlockPixels(mskPixMap);
            UnlockPixels(srcPixMap);
            
            // make a new sheep whose deadbounds are this rectangle (normalised to 0,0 at top left)
            
            chunkDstRect = chunkSrcRect;
            OffsetRect(&chunkDstRect, -chunkSrcRect.left, -chunkSrcRect.top);
            
            
            newSheep = (SheepToken *)NewPtr(sizeof(SheepToken));
            
            newSheep->readyToDie = false;
            newSheep->layer = theSheep->layer;
            newSheep->position.x = theSheep->position.x + (chunkSrcRect.left + chunkSrcRect.right - firstSheepBounds.right) / 2;
            newSheep->position.y = theSheep->position.y + (chunkSrcRect.top + chunkSrcRect.bottom - firstSheepBounds.bottom) / 2;
            
            if (chunkSrcRect.right & 1) newSheep->position.x++;
            if (chunkSrcRect.bottom & 1) newSheep->position.y++; // i had a case where a sheep was wandering up/left.
            
            burstVector.x = newSheep->position.x - theSheep->position.x;
            burstVector.y = newSheep->position.y - theSheep->position.y;
            
            newSheep->velocity.x = theSheep->velocity.x + burstVector.x * 0.05;
            newSheep->velocity.y = theSheep->velocity.y - 0.5 + burstVector.y * 0.05;
            newSheep->deadBounds = chunkDstRect;
            newSheep->timesShot = theSheep->timesShot;
            newSheep->isBurning = theSheep->isBurning;
            
            err = NewGWorld(&newSheep->deadSprite,
                            16,
                            &chunkDstRect,
                            NULL,
                            NULL,
                            0);
            if (err) CleanUp(TRUE);
            
            err = NewGWorld(&newSheep->deadSpriteMaskWithoutOutline,
                            1,
                            &chunkDstRect,
                            NULL,
                            NULL,
                            0);
            if (err) CleanUp(TRUE);
    
            err = NewGWorld(&newSheep->deadSpriteMaskWithOutline,
                            1,
                            &chunkDstRect,
                            NULL,
                            NULL,
                            0);
            if (err) CleanUp(TRUE);
            
            err = NewGWorld(&newSheep->burnMask,
                            1,
                            &chunkDstRect,
                            NULL,
                            NULL,
                            0);
            if (err) CleanUp(TRUE);
            
            ClearGWorld(newSheep->deadSprite, whiteColor);
            ClearGWorld(newSheep->deadSpriteMaskWithoutOutline, whiteColor);
            ClearGWorld(newSheep->deadSpriteMaskWithOutline, whiteColor);
            ClearGWorld(newSheep->burnMask, whiteColor);
            
            if (g->baseSheep)
            {
                    thisSheep = g->baseSheep;
                    
                    if (thisSheep != theSheep)
                    {
                            while (thisSheep->next != theSheep)
                            {
                                    thisSheep = thisSheep->next;
                            }
                            
                            newSheep->next = theSheep;
                            theSheep->prev = newSheep;
                            thisSheep->next = newSheep;
                            newSheep->prev = thisSheep;
                    }else{
                            newSheep->next = thisSheep;
                            newSheep->prev = NULL;
                            thisSheep->prev = newSheep;
                            
                            g->baseSheep = newSheep;
                            
                    }
            }else{
                    
                    newSheep->prev = NULL;
                    newSheep->next = NULL;
                    g->baseSheep = newSheep;
            }
            
            // copy withmask, withoutmask, sprite and burnmask to new sheep
            
            SetGWorld(newSheep->deadSprite, NULL);
            
            LockPixels(mskPixMap);
            
            copSrcPixMap = GetGWorldPixMap(theSheep->deadSprite);
            copDstPixMap = GetGWorldPixMap(newSheep->deadSprite);
            
            LockPixels(copSrcPixMap);
            LockPixels(copDstPixMap);
            
            CopyMask(	(BitMap *)*copSrcPixMap,
                                    (BitMap *)*mskPixMap,
                                    (BitMap *)*copDstPixMap,
                                    &chunkSrcRect,
                                    &chunkSrcRect,
                                    &chunkDstRect);
            
            UnlockPixels(copSrcPixMap);
            UnlockPixels(copDstPixMap);
            
            SetGWorld(newSheep->deadSpriteMaskWithOutline, NULL);
            
            copSrcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
            copDstPixMap = GetGWorldPixMap(newSheep->deadSpriteMaskWithOutline);
            
            LockPixels(copSrcPixMap);
            LockPixels(copDstPixMap);
            
            CopyMask(	(BitMap *)*srcPixMap,
                                    (BitMap *)*mskPixMap,
                                    (BitMap *)*copDstPixMap,
                                    &chunkSrcRect,
                                    &chunkSrcRect,
                                    &chunkDstRect);
            
            UnlockPixels(copSrcPixMap);
            UnlockPixels(copDstPixMap);
            
            SetGWorld(newSheep->deadSpriteMaskWithoutOutline, NULL);
            
            copSrcPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithoutOutline);
            copDstPixMap = GetGWorldPixMap(newSheep->deadSpriteMaskWithoutOutline);
            
            LockPixels(copSrcPixMap);
            LockPixels(copDstPixMap);
            
            CopyMask(	(BitMap *)*copSrcPixMap,
                                    (BitMap *)*mskPixMap,
                                    (BitMap *)*copDstPixMap,
                                    &chunkSrcRect,
                                    &chunkSrcRect,
                                    &chunkDstRect);
            
            UnlockPixels(copSrcPixMap);
            UnlockPixels(copDstPixMap);
            
            SetGWorld(newSheep->burnMask, NULL);
            
            copSrcPixMap = GetGWorldPixMap(theSheep->burnMask);
            copDstPixMap = GetGWorldPixMap(newSheep->burnMask);
            
            LockPixels(copSrcPixMap);
            LockPixels(copDstPixMap);
            
            CopyMask(	(BitMap *)*copSrcPixMap,
                        (BitMap *)*mskPixMap,
                        (BitMap *)*copDstPixMap,
                        &chunkSrcRect,
                        &chunkSrcRect,
                        &chunkDstRect);
            
            UnlockPixels(copSrcPixMap);
            UnlockPixels(copDstPixMap);
            
            UnlockPixels(mskPixMap);
            
            // cut areas copied out of maskwith
            
            SetGWorld(theSheep->deadSpriteMaskWithOutline, NULL);
            
            copSrcPixMap = GetGWorldPixMap(g->theSheepType.splitMask);
            copDstPixMap = GetGWorldPixMap(theSheep->deadSpriteMaskWithOutline);
            
            LockPixels(copSrcPixMap);
            LockPixels(copDstPixMap);
            
            CopyBits(	(BitMap *)*copSrcPixMap,
                        (BitMap *)*copDstPixMap,
                        &chunkSrcRect,
                        &chunkSrcRect,
                        srcBic,
                        NULL);
            
            UnlockPixels(copSrcPixMap);
            UnlockPixels(copDstPixMap);
            
        }
        else // no black found in deadspritemaskwithoutline
        {
            doneCopying = true;
        }
        
    // end loop if donecopying is true
    }
    
    
    // get rid of the original - it's been entirely replaced. hah! who needs efficiency!
    
    theSheep->readyToDie = true;
    
    SetGWorld(storePort, storeDevice);
    
    if (numPieces >= 5)
            LambChopBonus();
}



void FloodFill (Ptr srcPixBaseAddr, Ptr mskPixBaseAddr, unsigned long srcRowBytes, unsigned long mskRowBytes, Point thePoint, Rect *pictureBounds, Rect *filledBounds)
{
	// pixmaps MUST BE LOCKED before we call this function
	
	// check pixel is in picture, then check pixel in mask has not yet been filled, then see if it needs filling
	
	// note: i'm sure SeedCFill will be more efficient, this is totally unoptimised,
	// but I was having difficulty with it and wanted just to get the damn thing working
	
	if ( MyPtInRect(&thePoint, pictureBounds) )
	{
		
		if ( srcPixBaseAddr[(thePoint.h / 8) + (thePoint.v * srcRowBytes)] & (128 >> (thePoint.h % 8)) )
		{
			
			if ( !(mskPixBaseAddr[(thePoint.h / 8) + (thePoint.v * mskRowBytes)] & (128 >> (thePoint.h % 8))) )
			{
				// it needs filling, so fill it
				
				mskPixBaseAddr[(thePoint.h / 8) + (thePoint.v * mskRowBytes)] += (128 >> (thePoint.h % 8));
				
				
				if (thePoint.h < filledBounds->left) filledBounds->left = thePoint.h;
				if (thePoint.v < filledBounds->top) filledBounds->top = thePoint.v;
				if (thePoint.h >= filledBounds->right) filledBounds->right = thePoint.h + 1;
				if (thePoint.v >= filledBounds->bottom) filledBounds->bottom = thePoint.v + 1;
				
				thePoint.h += 1;
				FloodFill(srcPixBaseAddr, mskPixBaseAddr, srcRowBytes, mskRowBytes, thePoint, pictureBounds, filledBounds);
				thePoint.h -= 2;
				FloodFill(srcPixBaseAddr, mskPixBaseAddr, srcRowBytes, mskRowBytes, thePoint, pictureBounds, filledBounds);
				thePoint.h += 1;
				
				thePoint.v += 1;
				FloodFill(srcPixBaseAddr, mskPixBaseAddr, srcRowBytes, mskRowBytes, thePoint, pictureBounds, filledBounds);
				thePoint.v -= 2;
				FloodFill(srcPixBaseAddr, mskPixBaseAddr, srcRowBytes, mskRowBytes, thePoint, pictureBounds, filledBounds);
			}
		}
	}
}





void RemoveDeadStuff (void)
{
    SheepToken	*thisSheep;
    SheepToken	*thatSheep;
    ShotEffect	*thisShEffect;
    ShotEffect	*thatShEffect;
    ScoreEffect	*thisScEffect;
    ScoreEffect	*thatScEffect;
    
    thisSheep = g->baseSheep;
    thisScEffect = g->baseScoreEffect;
    thisShEffect = g->baseShotEffect;
    
    while (thisSheep)
    {
        thatSheep = thisSheep->next;
        
        if (thisSheep->readyToDie)
            RemoveSheep(thisSheep);
        
        thisSheep = thatSheep;
    }
    
    while (thisScEffect)
    {
        thatScEffect = thisScEffect->next;
        
        if (thisScEffect->readyToDie)
            RemoveScoreEffect(thisScEffect);
        
        thisScEffect = thatScEffect;
    }
    
    while (thisShEffect)
    {
        thatShEffect = thisShEffect->next;
        
        if (thisShEffect->readyToDie)
            RemoveShotEffect(thisShEffect);
        
        thisShEffect = thatShEffect;
    }
}


void StateSwitch(short theState)
{
    g->gameState = theState;
    g->lastStateSwitchTime = GetCurrentEventTime();
    
    if (theState == kPlaying)
        PlaySound(kBleepChannel, g->sounds.baa);
    else
        PlaySound(kBleepChannel, g->sounds.stateSwitch);
}

// bonuses





void ChainLengthUp(void)
{
    g->bonusChainLength++;
    
    if (g->bonusChainLength == kBClevel1 ||
        g->bonusChainLength == kBClevel2 ||
        g->bonusChainLength == kBClevel3 ||
        g->bonusChainLength == kBClevel4 ||
        g->bonusChainLength == kBClevel5 )
        
        ChainUpBonus(); // chain up bonuses do not add to the bonus chain length
    
    if (g->bonusChainLength == 1) PlaySound(kChainChannel, g->sounds.clank);
}

short GetChainLevel(void)
{
    if (g->bonusChainLength < kBClevel1)
        return 0;
    
    if (g->bonusChainLength < kBClevel2)
        return 1;
    
    if (g->bonusChainLength < kBClevel3)
        return 2;
    
    if (g->bonusChainLength < kBClevel4)
        return 3;
    
    if (g->bonusChainLength < kBClevel5)
        return 4;
    
    return 5;
}

void BreakBonusChain(void)
{
    if (g->bonusChainLength && (g->frameTime - g->lastBonusTime < BONUS_TIME_LENGTH)) PlaySound(kChainChannel, g->sounds.breakchain);
    
    g->bonusChainLength = 0;
    g->lastBonus = kNoBonus;
    g->lastBonusData = 0;
    g->lastBonusDataLife = FALSE;
}
    

void ChainUpBonus(void)
{
    UInt32 bonus = pow(10, 1 + GetChainLevel());
    
    g->lastBonusData += bonus;
    if (g->lastBonusDataLife)
        g->theScoreStuff.livesLeft++;
    
    g->theScoreStuff.score += g->lastBonusData;
    
    g->lastBonus = kChainUpBonus;
    
    PlaySound(kChainChannel, g->sounds.clank);
}



void LambChopBonus(void)
{
    bool chainUp = FALSE;
    
    g->theScoreStuff.livesLeft++;
    
    if (g->frameTime - g->lastBonusTime < BONUS_TIME_LENGTH)
    {
        chainUp = TRUE;
        g->lastBonusData += 0;
        g->theScoreStuff.score += g->lastBonusData;
        g->lastBonusDataLife = TRUE;
    }
    else
    {
        g->bonusChainLength = 0;
        g->lastBonusData = 0;
        g->lastBonusDataLife = TRUE;
    }
    
    g->lastBonusTime = g->frameTime;
    g->lastBonus = kLambChopBonus;
    
    PlaySound(kBonusChannel, g->sounds.LCBonus);
    g->bonusAlreadyThisFrame = TRUE;
    
    if (chainUp)
        ChainLengthUp();
}

void DoPointsBonus(UInt32 bonus, short bonusTypeConstant)
{
    bool chainUp = FALSE;
    
    if (g->frameTime - g->lastBonusTime < BONUS_TIME_LENGTH)
    {
        chainUp = TRUE;
        g->lastBonusData += bonus;
        if (g->lastBonusDataLife)
            g->theScoreStuff.livesLeft++;
    }
    else
    {
        g->bonusChainLength = 0;
        g->lastBonusData = bonus;
        g->lastBonusDataLife = FALSE;
    }
    
    g->theScoreStuff.score += g->lastBonusData;
    
    g->lastBonusTime = g->frameTime;
    g->lastBonus = bonusTypeConstant;
    
    if (!g->bonusAlreadyThisFrame)
    {
        if (g->lastBonusDataLife)
            PlaySound(kBonusChannel, g->sounds.LCBonus);
        else
            PlaySound(kBonusChannel, g->sounds.SCBonus);
        
        g->bonusAlreadyThisFrame = TRUE;
    }
    
    if (chainUp)
        ChainLengthUp();
}

void SwissCheeseBonus(short numDivFifty) // not div 50, now 25
{
    UInt32 bonus = 500 * numDivFifty * numDivFifty;
    
    DoPointsBonus(bonus, kSwissCheeseBonus);
}

void ShishKebabBonus(short numHit)
{
    UInt32 bonus = 100 * numHit * numHit * g->theWeapon->scoreMultiplier;
    
    DoPointsBonus(bonus, kShishKebabBonus);
}

void HighFlierBonus(short altitudeAbove) // altitudeAboveOneScreenAbove, in pixels, approx
{
    UInt32 bonus = ((altitudeAbove / 25) + 1) * ((altitudeAbove / 25) + 1) * 50;
    
    DoPointsBonus(bonus, kHighFlierBonus);
}

void JammyDodgerBonus(short distanceOffscreen)
{
    UInt32 bonus = ((distanceOffscreen/3)+1) * ((distanceOffscreen/3)+1) * 10 * g->theWeapon->scoreMultiplier;
    
    DoPointsBonus(bonus, kJammyDodgerBonus);
}


