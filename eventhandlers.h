/*
 *  eventhandlers.h
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 10 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

void InstallEventHandlers(void);

void StartTimer(void);
void ReStartTimer(void);


OSStatus DoCloseMainWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus AutoPauseIfInGame(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus ActivateMyWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus DeactivateMyWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus DrawMainWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus DrawInstructionsWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus DrawAboutWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus AskSwitchHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus PrefsHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus HighScoresHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus LevelSelectHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus AboutHandleMouse(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus InstructionsHandleMouse(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus HighScoreHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus GameOverHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus AutoPauseHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);

OSStatus HandleMenu(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
OSStatus HandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data);
pascal void DoFrame (EventLoopTimerRef  theTimer, void *userData);



void InterfaceMousePos(void);

void Prefs(void);
void SetPrefs(void);
void HighScores(void);
void About(void);
void Instructions(void);
void LevelSelect(void);
void HighScore(void);
void GameOver(void);
void AutoPause(void);
void AskSwitchDepth(void);

void GetCompensatedMouse(Point *pt);
void WindowSizeHack(Boolean repos);

Point GetSubMainWindowPos(WindowRef theWindow, short stdHeightBelowMain);


