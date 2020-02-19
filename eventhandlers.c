/*
 *  eventhandlers.c
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 10 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "mafftypes.h"
#include "graphics.h"
#include "game.h"
#include "eventhandlers.h"

#define PRF_CUR kCFPreferencesCurrentApplication

extern GlobalStuff *g;

extern void CleanUp(bool instaQuit);
extern void SavePreferences(void);
extern void LoadSounds(void);
extern void FinishSounds(void);
extern void UpdateHighScores(void);
extern void UnloadHighScores(void);
extern void DefaultHighScores(void);
extern void ProcessAndDrawFire (GWorldPtr theGWorld);
extern void PlaySound(short channel, Handle sound);
extern void ChangeDepth(void);
extern void ChangeDepthBack(void);

void InstallEventHandlers(void)
{
    OSErr		err;
    EventTypeSpec	eventType[5];
    EventHandlerUPP	theUPP;
    
    // main window level:
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowClose;
    theUPP = NewEventHandlerUPP(DoCloseMainWindow);
    err = InstallWindowEventHandler(g->theWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowDeactivated;
    eventType[1].eventClass = kEventClassWindow;
    eventType[1].eventKind = kEventWindowCollapsed;
    eventType[2].eventClass = kEventClassWindow;
    eventType[2].eventKind = kEventWindowHidden;
    eventType[3].eventClass = kEventClassWindow;
    eventType[3].eventKind = kEventWindowDragStarted;
    eventType[4].eventClass = kEventClassMenu;
    eventType[4].eventKind = kEventMenuBeginTracking;
    theUPP = NewEventHandlerUPP(DeactivateMyWindow);
    err = InstallWindowEventHandler(g->theWindow, theUPP, 5, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowActivated;
    eventType[1].eventClass = kEventClassWindow;
    eventType[1].eventKind = kEventWindowExpanded;
    eventType[2].eventClass = kEventClassWindow;
    eventType[2].eventKind = kEventWindowShown;
    eventType[3].eventClass = kEventClassWindow;
    eventType[3].eventKind = kEventWindowDragCompleted;
    eventType[4].eventClass = kEventClassMenu;
    eventType[4].eventKind = kEventMenuEndTracking;
    theUPP = NewEventHandlerUPP(ActivateMyWindow);
    err = InstallWindowEventHandler(g->theWindow, theUPP, 5, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowUpdate;
    theUPP = NewEventHandlerUPP(DrawMainWindow);
    err = InstallWindowEventHandler(g->theWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // display switcher
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(AskSwitchHandleCommand);
    err = InstallWindowEventHandler(g->askSwitchWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    
    // prefs window:
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(PrefsHandleCommand);
    err = InstallWindowEventHandler(g->prefsWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // high scores window
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(HighScoresHandleCommand);
    err = InstallWindowEventHandler(g->highScoresWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    
    
    // level select window:
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(LevelSelectHandleCommand);
    err = InstallWindowEventHandler(g->levelSelectWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // about window
    
    eventType[0].eventClass = kEventClassMouse;
    eventType[0].eventKind = kEventMouseUp;
    theUPP = NewEventHandlerUPP(AboutHandleMouse);
    err = InstallWindowEventHandler(g->aboutWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowUpdate;
    theUPP = NewEventHandlerUPP(DrawAboutWindow);
    err = InstallWindowEventHandler(g->aboutWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // instruct window
    
    eventType[0].eventClass = kEventClassMouse;
    eventType[0].eventKind = kEventMouseUp;
    theUPP = NewEventHandlerUPP(InstructionsHandleMouse);
    err = InstallWindowEventHandler(g->instructionsWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassWindow;
    eventType[0].eventKind = kEventWindowUpdate;
    theUPP = NewEventHandlerUPP(DrawInstructionsWindow);
    err = InstallWindowEventHandler(g->instructionsWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // high score dialog
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(HighScoreHandleCommand);
    err = InstallWindowEventHandler(g->highScoreWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // game over dialog
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(GameOverHandleCommand);
    err = InstallWindowEventHandler(g->gameOverWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // autopause window
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(AutoPauseHandleCommand);
    err = InstallWindowEventHandler(g->autoPauseWindow, theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    // application level:
    
    eventType[0].eventClass = kEventClassCommand;
    eventType[0].eventKind = kEventCommandProcess;
    theUPP = NewEventHandlerUPP(HandleCommand);
    err = InstallApplicationEventHandler(theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassMenu;
    eventType[0].eventKind = kEventMenuEndTracking;
    theUPP = NewEventHandlerUPP(HandleMenu);
    err = InstallApplicationEventHandler(theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    eventType[0].eventClass = kEventClassApplication;
    eventType[0].eventKind = kEventAppActivated;
    theUPP = NewEventHandlerUPP(AutoPauseIfInGame);
    err = InstallApplicationEventHandler(theUPP, 1, &eventType[0], NULL, NULL);
    if (err) CleanUp(TRUE);
    
    StartTimer();
}

void StartTimer(void)
{
    EventLoopRef	mainLoop;
    EventLoopTimerUPP	timerUPP;
    double		frameTime;
    
    if (g->pref.turbo)
        frameTime = 0.01667 * kEventDurationSecond;
    else
        frameTime = 0.03333 * kEventDurationSecond;
    
    mainLoop = GetMainEventLoop();
    timerUPP = NewEventLoopTimerUPP(DoFrame);
    InstallEventLoopTimer(mainLoop, 0, frameTime, timerUPP, NULL, &g->theTimer);
}

void ReStartTimer(void) // call when g->pref.turbo changes
{
    if (g->theTimer) RemoveEventLoopTimer(g->theTimer);
    
    StartTimer();
}

//(const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon)

OSStatus DoCloseMainWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    QuitApplicationEventLoop();
    
    return noErr;
}

OSStatus AutoPauseIfInGame(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    AutoPause(); // the ifInGame bit is checked inside AutoPause()
    
    return noErr;
}


OSStatus ActivateMyWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    g->windowActive = 1;
    
    if (g->alreadyAutoPaused)
        g->alreadyAutoPaused = FALSE;
    else
        AutoPause();
    
    return noErr;
}

OSStatus DeactivateMyWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    g->windowActive = 0;
    
    return noErr;
}

OSStatus DrawMainWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    BeginUpdate(g->theWindow);
    
    DrawBorder();
    
    DrawGWorldToWindow(g->swapGWorld, g->theWindow);
    
    EndUpdate(g->theWindow);
    
    return eventNotHandledErr;
}

OSStatus DrawInstructionsWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    BeginUpdate(g->instructionsWindow);
    
    DrawGWorldToWindow(g->instructionsGWorld, g->instructionsWindow);
    
    EndUpdate(g->instructionsWindow);
    
    return eventNotHandledErr;
}

OSStatus DrawAboutWindow(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    BeginUpdate(g->aboutWindow);
    
    DrawGWorldToWindow(g->aboutGWorld, g->aboutWindow);
    
    EndUpdate(g->aboutWindow);
    
    return eventNotHandledErr;
}

// depth check


OSStatus AskSwitchHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand		commandStruct;
    bool		somethingHappened;
    
    ControlID		cntlid;
    ControlRef		theControl;
    
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'chan':
            g->pref.switchDepth = TRUE;
            somethingHappened = TRUE;
            break;
        
        case 'nooo':
            g->pref.switchDepth = FALSE;
            somethingHappened = TRUE;
            break;
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    if (somethingHappened)
    {
        cntlid.signature = 'alwa';
        cntlid.id = 1;
        GetControlByID(g->askSwitchWindow, &cntlid, &theControl);
        
        if ( GetControl32BitValue(theControl) )
        {
            if (g->pref.switchDepth)
                CFPreferencesSetAppValue(CFSTR("switchDepth"), kCFBooleanTrue, PRF_CUR);
            else
                CFPreferencesSetAppValue(CFSTR("switchDepth"), kCFBooleanFalse, PRF_CUR);
        }
        
        if (g->pref.switchDepth == TRUE)
            ChangeDepth();
        
        HideWindow(g->askSwitchWindow);
        SelectWindow(g->theWindow);
    }
    
    return noErr;
}





// prefs window



OSStatus PrefsHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            SetPrefs();		// this function must load/finish sounds if "sound" changes.
            HideWindow(g->prefsWindow);
            SelectWindow(g->theWindow);
            AutoPause();
            break;
        
        case 'not!':
            HideWindow(g->prefsWindow);
            SelectWindow(g->theWindow);
            AutoPause();
            break;
            
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
}

// hiscores window

OSStatus HighScoresHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    bool	redraw = FALSE;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            HideWindow(g->highScoresWindow);
            SelectWindow(g->theWindow);
            
            if (g->startNewGameImmediately)
            {
                NewGame(g->startNewGameImmediately);
                g->startNewGameImmediately = 0;
            }
            else
                AutoPause();
            break;
        
        case 'rese':
            UnloadHighScores();
            DefaultHighScores(); // also saves new values
            HighScores();
            SysBeep(1);
            break;
        
        case 'hisc':
            g->scores.lastLookedAt = 1;
            redraw = TRUE;
            HighScores();
            break;
        
        case 'moki':
            g->scores.lastLookedAt = 2;
            redraw = TRUE;
            HighScores();
            break;
        
        case 'moch':
            g->scores.lastLookedAt = 3;
            redraw = TRUE;
            HighScores();
            break;
        
        case 'loju':
            g->scores.lastLookedAt = 4;
            redraw = TRUE;
            HighScores();
            break;
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    if (!redraw)
        return noErr;
    else
        return eventNotHandledErr;
}

// level select window

OSStatus LevelSelectHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)///////////////////////
{
    ControlID		cntlid;
    ControlRef		theControl;
    unsigned char	theString[255];
    CFStringRef		theCFString = NULL;
    Size		theSize, outSize;
    HICommand		commandStruct;
    short		levelNumber;
    short		i;
    Level		*theLevel;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            
            ///////////////////////////////////////////////////////////////////////////////////////////////
            
            // parse input, determine level
            
            cntlid.signature = 'levl';
            cntlid.id = 0;
            GetControlByID(g->levelSelectWindow, &cntlid, &theControl);
            
            GetControlDataSize(theControl, kControlEditTextPart, kControlEditTextTextTag, &theSize);
            
            if (theSize > 255)
                theSize = 255;
            
            GetControlData(theControl, kControlEditTextPart, kControlEditTextTextTag, theSize, theString, &outSize);
            
            if (outSize < 255)
                theString[outSize] = 0;
            else
                theString[254] = 0;
            
            if (outSize == 0)
            {
                SetKeyboardFocus(g->levelSelectWindow, theControl, kControlEditTextPart);
                SysBeep(1);
            }
            else
            {
                theCFString = CFStringCreateWithCString(NULL, theString, GetApplicationTextEncoding());
                levelNumber = CFStringGetIntValue(theCFString);
                CFRelease(theCFString);
                
                i = 1;
                theLevel = g->baseLevel;
                
                while (i < levelNumber && theLevel)
                {
                    i++;
                    theLevel = theLevel->next;
                }
                
                if (theLevel && levelNumber >= 1)
                {
                    EndGame(FALSE);
                    g->startNewGameImmediately = levelNumber;
                    
                    HideWindow(g->autoPauseWindow);
                    HideWindow(g->prefsWindow);
                    HideWindow(g->highScoresWindow);
                    HideWindow(g->gameOverWindow);
                    HideWindow(g->levelSelectWindow);
                    HideWindow(g->instructionsWindow);
                    HideWindow(g->aboutWindow);
                    HideWindow(g->instructionsWindow);
                    
                    SelectWindow(g->theWindow);
                }
                else
                {
                    SetKeyboardFocus(g->levelSelectWindow, theControl, kControlEditTextPart);
                    SysBeep(1);
                }
            }
            break;
        
        case 'not!':
            HideWindow(g->levelSelectWindow);
            SelectWindow(g->theWindow);
            AutoPause();
            break;
            
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
    
}


// about box

OSStatus AboutHandleMouse(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    if (GetCurrentEventTime() - g->lastAboutStyleWindowTime <= 0.25)
        return noErr;
    
    if (g->firstRunDoAbout)
    {
        Instructions();
        g->firstRunDoAbout = FALSE;
    }
    else
    {
        HideWindow(g->aboutWindow);
        ShowWindow(g->theWindow);
        SelectWindow(g->theWindow);
    }
    
    return noErr;
}

// instructions

OSStatus InstructionsHandleMouse(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    if (GetCurrentEventTime() - g->lastAboutStyleWindowTime <= 0.25)
        return noErr;
    
    HideWindow(g->instructionsWindow);
    SelectWindow(g->theWindow);
    AutoPause();
    
    return noErr;
}

// high score dialog

OSStatus HighScoreHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            UpdateHighScores(); // get name, insert names and scores into HighScoreList, save HighScoreList.
            
            HideWindow(g->highScoreWindow);
            HighScores();
            break;
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
}

// high score dialog

OSStatus GameOverHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            
            g->theScoreStuff.score = 0;
            g->theScoreStuff.highestMultiplier = 0;
            g->theScoreStuff.longestJuggle = 0;
            g->theScoreStuff.sheepKilledTotal = 0;
            
            HideWindow(g->gameOverWindow);
            SelectWindow(g->theWindow);
            break;
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
}

// autopause



OSStatus AutoPauseHandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'ok  ':
            g->alreadyAutoPaused = TRUE;
            HideWindow(g->autoPauseWindow);
            SelectWindow(g->theWindow);
            break;
        
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
}





// Application-level

OSStatus HandleMenu(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    SetCursor(*g->crosshair);
    return eventNotHandledErr;
}




OSStatus HandleCommand(EventHandlerCallRef nextHandler, EventRef theEvent, void *data)
{
    HICommand	commandStruct;
    
    GetEventParameter(theEvent, kEventParamDirectObject, 
                    typeHICommand, NULL, sizeof(HICommand), 
                    NULL, &commandStruct);
    
    switch (commandStruct.commandID)
    {
        case 'abou':
            About();
            break;
        
        case 'pref':
            Prefs();
            break;
        
        case 'newg':
            EndGame(FALSE);
            HideWindow(g->autoPauseWindow);
            HideWindow(g->prefsWindow);
            HideWindow(g->highScoresWindow);
            HideWindow(g->gameOverWindow);
            HideWindow(g->levelSelectWindow);
            HideWindow(g->instructionsWindow);
            HideWindow(g->aboutWindow);
            HideWindow(g->instructionsWindow);
            g->startNewGameImmediately = 1;
            break;
        
        case 'chlv':
            LevelSelect();
            break;
        
        case 'hisc':
            HighScores();
            break;
        
        case 'inst':
            Instructions();
            break;
        
        case 'endg':
            if (g->inGame)
            {
                g->alreadyAutoPaused = TRUE;
                HideWindow(g->autoPauseWindow);
                HideWindow(g->prefsWindow);
                HideWindow(g->highScoresWindow);
                HideWindow(g->gameOverWindow);
                HideWindow(g->levelSelectWindow);
                HideWindow(g->instructionsWindow);
                HideWindow(g->aboutWindow);
                HideWindow(g->instructionsWindow);
                EndGame(FALSE);
            }
            break;
        
        case 'mini':
            // 'mini' command should be generated after the kEventMenuEndTracking or whatever it is that AutoPauses
            
            g->alreadyAutoPaused = TRUE;
            HideWindow(g->autoPauseWindow);
            
            return eventNotHandledErr;
            break;
            
        default:
            return eventNotHandledErr;
            break;
    }
    
    return noErr;
}


pascal void DoFrame (EventLoopTimerRef  theTimer, void *userData)
{
    static EventTime lastTime, thisTime;
//    Str255 string;				// fps counter
    
    lastTime = thisTime;
    thisTime = GetCurrentEventTime();
    
    if (g->windowActive)
    {
        if (g->startNewGameImmediately)
        {
            NewGame(g->startNewGameImmediately);
            g->startNewGameImmediately = 0;
        }
        
        CheckKeys();
        
        if (g->inGame)
        {
            g->sheepArrowsThisFrame = 0;
            
            switch (g->gameState)
            {
                case kLevelStart:
                    
                    DrawBackground(g->swapGWorld);
                    DrawStuff(g->swapGWorld);
                    DrawScore(g->swapGWorld);
                    
                    DrawLevelStartStuff(g->swapGWorld);
                    g->bonusChainLength = 0;
                    
                    break;
                
                case kPlaying:
                    
                    if (! (alphaLock & GetCurrentKeyModifiers()) )
                        OneFrame();
                    else
                        DrawPausedStuff(g->swapGWorld);
                    break;
                
                case kLevelEnd:
                    
                    DrawBackground(g->swapGWorld);
                    DrawStuff(g->swapGWorld);
                    DrawScore(g->swapGWorld);
                    
                    DrawLevelEndStuff(g->swapGWorld);
                    break;
                
                case kGameOverScreen:
                    
                    g->frameTime = GetCurrentEventTime();
                    
                    ProcessSheep();
                    ProcessScoreEffects();
                    RemoveDeadStuff();
                    
                    DrawBackground(g->swapGWorld);
                    DrawStuff(g->swapGWorld);
                    
                    if (g->pref.fire == kOn)
                        ProcessAndDrawFire(g->swapGWorld);
                    
                    DrawScoreEffects(g->swapGWorld);
                    DrawScore(g->swapGWorld);
                    DrawBonus(g->swapGWorld);
                    
                    DrawGameOverScreenStuff(g->swapGWorld);
                    break;
                
                case kCompletedScreen:
                    
                    DrawBackground(g->swapGWorld);
                    DrawStuff(g->swapGWorld);
                    
                    if (g->pref.fire == kOn)
                        ProcessAndDrawFire(g->swapGWorld);
                    
                    DrawScoreEffects(g->swapGWorld);
                    DrawScore(g->swapGWorld);
                    DrawBonus(g->swapGWorld);
                    
                    DrawCompletedScreenStuff(g->swapGWorld);
                    break;
            }
        }
        else
        {
            InterfaceMousePos();
            DrawInterface(g->swapGWorld);
            
            if (g->askSwitchDepthPreferences)
                AskSwitchDepth();
            
            if (g->firstRunDoAbout)
                Instructions();
        }
    }
    
    CheckKeys(); // should trap for any presses of [esc] - brings up instructions
    
    DrawGWorldToWindow(g->swapGWorld, g->theWindow);
    
    g->fps = 1/(thisTime - lastTime);
//    NumToString(g->fps, &string[0]);			// fps counter
//    MoveTo(20,60);					// fps counter
//    DrawString(string);					// fps counter (like graph)
//    MoveTo(10,70);LineTo(10 * (g->fps+1), 70);		// fps counter (like graph)
}

// in the way

void InterfaceMousePos(void)
{
    Point		mouseLoc;
    GWorldPtr		currentPort;
    GDHandle		currentDevice;
    
    if (!g->windowActive) // leave this in, I might still need it
        return;
    
    GetGWorld(&currentPort, &currentDevice);
    
    if (GetWindowPort(g->theWindow) != currentPort)
        return;
    
    GetCompensatedMouse(&mouseLoc);
    
    if (mouseLoc.h >= 210 && mouseLoc.h < 410)
    {
        if (mouseLoc.v >= 110 && mouseLoc.v < 330)
        {
            if ( !Button() && (g->mouseOverOption != (1 + (mouseLoc.v - 110) / 55)) )
            {
                PlaySound(kBleepChannel, g->sounds.weaponSwitch);
            }
            g->mouseOverOption = 1 + (mouseLoc.v - 110) / 55;
        }
        else
            g->mouseOverOption = 0;
    }
    else
        g->mouseOverOption = 0;
    
    if (!g->clickedOnOption)
    {
        if (Button() && g->mouseOverOption && (g->mouseOverOption == (1 + (mouseLoc.v - 110) / 55)))
        {
            g->clickedOnOption = g->mouseOverOption;
            PlaySound(kBleepChannel, g->sounds.weaponSwitch);
        }
    }
    if(g->clickedOnOption)
    {
        if (!Button())
        {
            if (g->clickedOnOption == g->mouseOverOption)
            {
                switch (g->clickedOnOption)
                {
                    case kNewGame:
                        NewGame(1);
                        PlaySound(kBleepChannel, g->sounds.weaponSwitch);
                        break;
                    
                    case kHighScores:
                        HighScores();
                        PlaySound(kBleepChannel, g->sounds.weaponSwitch);
                        break;
                    
                    case kPreferences:
                        Prefs();
                        PlaySound(kBleepChannel, g->sounds.weaponSwitch);
                        break;
                    
                    case kQuit:
                        QuitApplicationEventLoop();
                        PlaySound(kBleepChannel, g->sounds.weaponSwitch);
                        break;
                    
                    default:
                        break;
                }
            }
            g->clickedOnOption = 0;
        }
    }
}

void Prefs(void)//////////////////////////////////////////////////////////////////////
{
    ControlID		cntlid;
    ControlRef		theControl;
    
    Boolean		value, valid;
    
    Point		pos;
    
    pos = GetSubMainWindowPos(g->prefsWindow, 100);
    
    MoveWindow(g->prefsWindow, pos.h, pos.v, FALSE);
    
    cntlid.signature = 'blud';
    cntlid.id = 1;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if (!IsValidControlHandle(theControl)) SysBeep(1);
    
    if (g->pref.blood == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'fire';
    cntlid.id = 2;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if (g->pref.fire == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'soun';
    cntlid.id = 3;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if (g->pref.sound == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'turb';
    cntlid.id = 4;
    GetControlByID (g->prefsWindow, &cntlid, &theControl);
    
    if (g->pref.turbo == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'shot';
    cntlid.id = 6;
    GetControlByID (g->prefsWindow, &cntlid, &theControl);
    
    if (g->pref.shotFX == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'bord';
    cntlid.id = 0;
    GetControlByID (g->prefsWindow, &cntlid, &theControl);
    
    if (g->pref.border == TRUE)
        SetControl32BitValue(theControl, 1);
    else
        SetControl32BitValue(theControl, 0);
    
    cntlid.signature = 'swit';
    cntlid.id = 5;
    GetControlByID (g->prefsWindow, &cntlid, &theControl);
    
    value = CFPreferencesGetAppBooleanValue(CFSTR("switchDepth"), PRF_CUR, &valid);
    
    if (valid)
    {
        if (value == TRUE)
            SetControl32BitValue(theControl, 1);
        else
            SetControl32BitValue(theControl, 0);
    }
    
    DrawControls(g->prefsWindow);
    ShowWindow(g->prefsWindow);
    SelectWindow(g->prefsWindow);
}

void SetPrefs(void)
{
    ControlID		cntlid;
    ControlRef		theControl;
    
    cntlid.signature = 'blud';
    cntlid.id = 1;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.blood = TRUE;
    else
        g->pref.blood = FALSE;
    
    cntlid.signature = 'fire';
    cntlid.id = 2;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.fire = TRUE;
    else
        g->pref.fire = FALSE;
    
    cntlid.signature = 'soun';
    cntlid.id = 3;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.sound = TRUE;
    else
        g->pref.sound = FALSE;
    
    cntlid.signature = 'turb';
    cntlid.id = 4;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.turbo = TRUE;
    else
        g->pref.turbo = FALSE;
    
    ReStartTimer();
    
    cntlid.signature = 'shot';
    cntlid.id = 6;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.shotFX = TRUE;
    else
        g->pref.shotFX = FALSE;
    
    cntlid.signature = 'bord';
    cntlid.id = 0;
    GetControlByID(g->prefsWindow, &cntlid, &theControl);
    
    if ( GetControl32BitValue(theControl) )
        g->pref.border = TRUE;
    else
        g->pref.border = FALSE;
    
    WindowSizeHack(FALSE);
    
    cntlid.signature = 'swit';
    cntlid.id = 5;
    GetControlByID (g->prefsWindow, &cntlid, &theControl);
    
    switch ( GetControl32BitValue(theControl) )
    {
        case 0:
            CFPreferencesSetAppValue(CFSTR("switchDepth"), kCFBooleanFalse, PRF_CUR);
            g->pref.switchDepth = FALSE;
            break;
        case 1:
            CFPreferencesSetAppValue(CFSTR("switchDepth"), kCFBooleanTrue, PRF_CUR);
            g->pref.switchDepth = TRUE;
            break;
        default:
            break;// leave indeterminate if nothing chosen
    }
    
    SavePreferences();
}

void HighScores()
{
    ControlID		cntlid;
    ControlRef		theControl;
    short		i = 0;
    CFStringRef		*strings = NULL, numberString;
    unsigned long	*scores = NULL;
    short		lastOne = -1;
    Str255		intermediateString;
    ControlFontStyleRec	fontStyle;
    
    Point		pos;
    
    pos = GetSubMainWindowPos(g->highScoresWindow, 20);
    
    MoveWindow(g->highScoresWindow, pos.h, pos.v, FALSE);
    
    cntlid.signature = 'tpmn';
    cntlid.id = 0;
    GetControlByID(g->highScoresWindow, &cntlid, &theControl);
    
    SetControl32BitValue(theControl, g->scores.lastLookedAt);
    
    switch (g->scores.lastLookedAt)
    {
        case 1:
            strings = &(g->scores.scoreName[0]);
            scores = &(g->scores.score[0]);
            lastOne = g->scores.lastScoreIndex;
            break;
        
        case 2:
            strings = &(g->scores.killedName[0]);
            scores = &(g->scores.sheepKilled[0]);
            lastOne = g->scores.lastKilledIndex;
            break;
        
        case 3:
            strings = &(g->scores.multName[0]);
            scores = &(g->scores.highestMultiplier[0]);
            lastOne = g->scores.lastMultIndex;
            break;
        
        case 4:
            strings = &(g->scores.juggleName[0]);
            scores = &(g->scores.longestJuggle[0]);
            lastOne = g->scores.lastJuggleIndex;
            break;
        
        default:
            SysBeep(1);
            break;
    }
    
    while(i < 10)
    {
        if (lastOne == i)
        {
            fontStyle.flags = kControlUseFaceMask;
            fontStyle.style = 1;
        }
        else
            fontStyle.flags = 0;
        
        cntlid.signature = 'name';
        cntlid.id = i;
        GetControlByID(g->highScoresWindow, &cntlid, &theControl);
        SetControlFontStyle(theControl, &fontStyle);
   //     SetControlTitleWithCFString(theControl, strings[i]);
        SetControlData(theControl, 0, kControlStaticTextCFStringTag, sizeof(CFStringRef), &strings[i]);
        
        NumToString(scores[i], intermediateString);
        numberString = CFStringCreateWithPascalString(NULL, intermediateString, GetApplicationTextEncoding());
        
        cntlid.signature = 'scor';
        cntlid.id = i;
        GetControlByID(g->highScoresWindow, &cntlid, &theControl);
        SetControlFontStyle(theControl, &fontStyle);
   //     SetControlTitleWithCFString(theControl, numberString);
        SetControlData(theControl, 0, kControlStaticTextCFStringTag, sizeof(CFStringRef), &numberString);
        CFRelease(numberString);
        
        i++;
    }
    
    DrawControls(g->highScoresWindow);
    ShowWindow(g->highScoresWindow);
    SelectWindow(g->highScoresWindow);
}

void About(void)
{
    Point  	pos;
    
    HideWindow(g->autoPauseWindow);
    
    pos = GetSubMainWindowPos(g->aboutWindow, 68);
    
    MoveWindow(g->aboutWindow, pos.h, pos.v, FALSE);
    
    ShowWindow(g->aboutWindow);
    DrawGWorldToWindow(g->aboutGWorld, g->aboutWindow);
    SelectWindow(g->aboutWindow);
    
    g->lastAboutStyleWindowTime = GetCurrentEventTime();
}

void Instructions(void)
{
    Point		pos;
    
    HideWindow(g->autoPauseWindow);
    
    if (g->firstRunDoAbout)
    {
        g->firstRunDoAbout = FALSE;
    }
    
    pos = GetSubMainWindowPos(g->aboutWindow, 68);
    
    MoveWindow(g->aboutWindow, pos.h, pos.v, FALSE);
    
    ShowWindow(g->aboutWindow);
    DrawGWorldToWindow(g->instructionsGWorld, g->aboutWindow);
    SelectWindow(g->aboutWindow);
    
    g->lastAboutStyleWindowTime = GetCurrentEventTime();
}

void LevelSelect(void)
{
    ControlID		cntlid;
    ControlRef		theControl;
    
    Point		pos;
    
    pos = GetSubMainWindowPos(g->levelSelectWindow, 90);
    
    MoveWindow(g->levelSelectWindow, pos.h, pos.v, FALSE);
    
    cntlid.signature = 'levl';
    cntlid.id = 0;
    GetControlByID(g->levelSelectWindow, &cntlid, &theControl);
    SetKeyboardFocus(g->levelSelectWindow, theControl, kControlEditTextPart);
    
    ShowWindow(g->levelSelectWindow);
    SelectWindow(g->levelSelectWindow);
}

void HighScore(void)
{
    ControlID		cntlid;
    ControlRef		theControl;
    Str255		theString;
    
    ControlFontStyleRec	boldStyle, plainStyle;
    
    Point		pos;
    
    plainStyle.flags = 0;
    boldStyle.flags = kControlUseFaceMask;
    boldStyle.style = 1;
    
    PlaySound(kBonusChannel, g->sounds.SCBonus);
    
    pos = GetSubMainWindowPos(g->highScoreWindow, 20);
    
    MoveWindow(g->highScoreWindow, pos.h, pos.v, FALSE);
    
    cntlid.signature = 'blob';
    cntlid.id = 128;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.score, theString);
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    if (g->theScoreStuff.score > g->scores.score[9])
        SetControlFontStyle(theControl, &boldStyle);
    else
        SetControlFontStyle(theControl, &plainStyle);
    
    cntlid.signature = 'blob';
    cntlid.id = 129;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.sheepKilledTotal, theString);
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    if (g->theScoreStuff.sheepKilledTotal > g->scores.sheepKilled[9])
        SetControlFontStyle(theControl, &boldStyle);
    else
        SetControlFontStyle(theControl, &plainStyle);
    
    cntlid.signature = 'blob';
    cntlid.id = 130;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.longestJuggle, theString);
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    if (g->theScoreStuff.longestJuggle > g->scores.longestJuggle[9])
        SetControlFontStyle(theControl, &boldStyle);
    else
        SetControlFontStyle(theControl, &plainStyle);
    
    cntlid.signature = 'blob';
    cntlid.id = 131;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.highestMultiplier, theString);
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    if (g->theScoreStuff.highestMultiplier > g->scores.highestMultiplier[9])
        SetControlFontStyle(theControl, &boldStyle);
    else
        SetControlFontStyle(theControl, &plainStyle);
    
    cntlid.signature = 'blob';
    cntlid.id = 132;
    GetControlByID(g->highScoreWindow, &cntlid, &theControl);
    
    CFStringGetPascalString(g->scores.lastName, theString, 255, GetApplicationTextEncoding());
    
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    SetKeyboardFocus(g->highScoreWindow, theControl, kControlEditTextPart);
    
    DrawControls(g->highScoreWindow);
    ShowWindow(g->highScoreWindow);
    SelectWindow(g->highScoreWindow);
}

void GameOver(void)
{
    ControlID		cntlid;
    ControlRef		theControl;
    Str255		theString;
    
    Point		pos;
    
    PlaySound(kBonusChannel, g->sounds.baa);
    
    pos = GetSubMainWindowPos(g->gameOverWindow, 20);
    
    MoveWindow(g->gameOverWindow, pos.h, pos.v, FALSE);
    
    g->scores.lastScoreIndex = -1;
    g->scores.lastKilledIndex = -1;
    g->scores.lastMultIndex = -1;
    g->scores.lastJuggleIndex = -1;
    
    cntlid.signature = 'blob';
    cntlid.id = 128;
    GetControlByID(g->gameOverWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.score, theString);
    
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    cntlid.signature = 'blob';
    cntlid.id = 129;
    GetControlByID(g->gameOverWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.sheepKilledTotal, theString);
    
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    cntlid.signature = 'blob';
    cntlid.id = 130;
    GetControlByID(g->gameOverWindow, &cntlid, &theControl);
    NumToString(g->theScoreStuff.longestJuggle, theString);
    
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    cntlid.signature = 'blob';
    cntlid.id = 131;
    GetControlByID(g->gameOverWindow, &cntlid, &theControl);
    if (g->theScoreStuff.score)						// no points => no sheep ever airborne
        NumToString(g->theScoreStuff.highestMultiplier, theString);
    else
    {
        theString[0] = 1;
        theString[1] = '0';
    }
    
    SetControlData( theControl,
                    kControlEditTextPart,
                    kControlEditTextTextTag,
                    theString[0],
                    (theString + 1));
    
    DrawControls(g->gameOverWindow);
    ShowWindow(g->gameOverWindow);
    SelectWindow(g->gameOverWindow);
}


void AutoPause(void)
{    
    Point		pos;
    
    pos = GetSubMainWindowPos(g->autoPauseWindow, 90);
    
    MoveWindow(g->autoPauseWindow, pos.h, pos.v, FALSE);

    if (g->theWindow == FrontNonFloatingWindow())
    {
        if (g->inGame)
        {
            if (! (alphaLock & GetCurrentKeyModifiers()) )
            {
                ShowWindow(g->autoPauseWindow);
                SelectWindow(g->autoPauseWindow);
            }
        }
    }
}
                     
void AskSwitchDepth(void)
{
    Point		pos;
    
    pos = GetSubMainWindowPos(g->askSwitchWindow, 100);
    
    MoveWindow(g->askSwitchWindow, pos.h, pos.v, FALSE);
    
    ShowWindow(g->askSwitchWindow);
    SelectWindow(g->askSwitchWindow);
    
    g->askSwitchDepthPreferences = FALSE;
}


void GetCompensatedMouse(Point *pt)
{
    GetMouse(pt);
    
    if (g->pref.border)
    {
        pt->h -= BORDER_WIDTH;
        pt->v -= BORDER_HEIGHT;
    }
}

void WindowSizeHack(Boolean repos)
{
    OSStatus err;
    
    if (g->pref.border)
        SizeWindow(g->theWindow, 620 + BORDER_WIDTH * 2, 420 + BORDER_HEIGHT * 2, TRUE);
    else
        SizeWindow(g->theWindow, 620, 420, TRUE);
    
    if (repos)
    {
        err = RepositionWindow(g->theWindow, NULL, kWindowCenterOnMainScreen);
        if (err != noErr)
            CleanUp(TRUE);
    }
    
    DrawBorder();
    
    DrawGWorldToWindow(g->swapGWorld, g->theWindow);
}


Point GetSubMainWindowPos(WindowRef theWindow, short stdHeightBelowMain)
{
    Point		pos;
    Rect		mainWinRect, thisWinRect;
    
    GetWindowBounds(g->theWindow, kWindowGlobalPortRgn, &mainWinRect);
    GetWindowBounds(theWindow, kWindowGlobalPortRgn, &thisWinRect);
    
    pos.h = mainWinRect.left + (g->pref.border?BORDER_WIDTH:0) + 310 - (thisWinRect.right - thisWinRect.left)/2;
    pos.v = mainWinRect.top + (g->pref.border?BORDER_HEIGHT:0) + stdHeightBelowMain;
    
    return pos;
}





















