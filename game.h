/*
 *  game.h
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 10 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include "mafftypes.h"




void NewGame (short level);
void NextLevel(void);
void EndGame(bool showNonHighScore);

void OneFrame (void);

void EmptyFireBuffer (void);

float		MyRandom				(float min, float max);
short		MyPtInRect				(Point *pt, Rect *rect);


void NewRandomSheep (void);
void NewSheep (short layer, float posx, float posy, float velx, float vely);

void RemoveSheep (SheepToken *theSheep);
void RemoveAllSheep (void);
void RemoveWeapon (Weapon *theWeapon);
void RemoveAllWeapons (void);

void AddShotEffect (Point where, short type);
void RemoveShotEffect (ShotEffect *theShotEffect);
void RemoveAllShotEffects (void);

void AddScoreEffect (short score, short multiplier, Point location);
void RemoveScoreEffect (ScoreEffect *theScoreEffect);
void RemoveAllScoreEffects (void);

void		ProcessSheep			(void);
void		ProcessScoreEffects 		(void);
void 		ProcessShotEffects 		(void);

void CheckKeys(void);

void		FireWeapon			(Point thePoint);
void		ShootSheep			(SheepToken *theSheep, Point hitPoint);
void		SplitSheep			(SheepToken *theSheep);
void		RemoveDeadStuff			(void);
void		FloodFill			(Ptr srcPixBaseAddr, Ptr mskPixBaseAddr, unsigned long srcRowBytes,
                                                unsigned long mskRowBytes, Point thePoint, Rect *pictureBounds, Rect *filledBounds);
                                                
void StateSwitch(short theState);

void ChainLengthUp(void);
short GetChainLevel(void);
void BreakBonusChain(void);
void ChainUpBonus(void);

void LambChopBonus(void);
void SwissCheeseBonus(short numDivFifty);
void ShishKebabBonus(short numHit);
void HighFlierBonus(short altitudeAbove);
void JammyDodgerBonus(short distanceOffscreen);




