/*
 *  loading.h
 *  nibMAFF
 *
 *  Created by wibble on Fri Sep 06 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

#include "mafftypes.h"

void LoadLoadingScreen(void);
void KillLoadingScreen(void);

void LoadInterface(void);

void LoadPreferences(void);
void DefaultPrefs(void);
void SavePreferences(void);

void LoadGlobalGraphics(void);
void LoadEveryThingElse(void);

void LoadLevels(void);
void LoadLevel(CFStringRef theLevelName);

Background *GetBackgroundWithName(CFStringRef theName);
void DumpBackgrounds(void);

void FillInStartPoints(Level *theLevel, CFArrayRef theArray);

void LoadLevelScenery(Level *theLevel, CFArrayRef theArray);
void AddScenery(Level *theLevel, CFStringRef sceneryName, pointFloat position, short layer);
SceneryType *FindSceneryType(CFStringRef sceneryName);
SceneryType *LoadSceneryType(CFStringRef sceneryName);

void RemoveAllLevels(void);
void RemoveAllScenery(void);

void LoadWeapons(void);
void LoadWeapon (short ID, float reload, float recoil, float scoreMultiplier, short topLeft, short botRight, CFStringRef name);

void LoadSound (CFStringRef name, long *nodeID);

void LoadPicture(CFStringRef name, GWorldPtr *theGWorld, bool flipped);
void LoadMask(CFStringRef name, GWorldPtr *theGWorld, bool flipped);
void DrawPictureToNewGWorld(FSSpec *fss, GWorldPtr *theGWorld, bool flipped);
void DrawMaskToNewGWorld(FSSpec *fss, GWorldPtr *theGWorld, bool flipped);


