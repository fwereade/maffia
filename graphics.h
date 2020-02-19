/*
 *  graphics.h
 *  nibMAFF
 *
 *  Created by wibble on Sun Sep 08 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

void DrawInterface (GWorldPtr theGWorld);

void InitSheepGWorld (GWorldPtr *srcGWorld, GWorldPtr *dstGWorld);

void DrawGWorldToWindow (GWorldPtr theGWorld, WindowRef theWindow);

void DrawBorder(void);

void ClearGWorld (GWorldPtr theGWorld, short colour);

void		DrawInterface			(GWorldPtr theGWorld);
void		DrawBackground			(GWorldPtr theGWorld);
void		DrawStuff			(GWorldPtr theGWorld);
void		DrawSheep			(GWorldPtr theGWorld, short layer);
void		DrawScenery			(GWorldPtr theGWorld, short layer);
void		DrawScoreEffects		(GWorldPtr theGWorld);
void		DrawShotEffects			(GWorldPtr theGWorld);
void		DrawScore			(GWorldPtr theGWorld);
void		DrawBonus			(GWorldPtr theGWorld);


void DrawLevelStartStuff(GWorldPtr theGWorld);
void DrawLevelEndStuff(GWorldPtr theGWorld);
void DrawGameOverScreenStuff(GWorldPtr theGWorld);
void DrawCompletedScreenStuff(GWorldPtr theGWorld);
void DrawPausedStuff(GWorldPtr theGWorld);

void AddSheepArrow(short posx);

