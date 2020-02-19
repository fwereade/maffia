#include <Carbon/Carbon.h>

#include "main.h"



extern void FinishSounds(void);
extern void RemoveAllLevels(void);
extern void RemoveAllScenery(void);
extern void CheckDepth(void);
extern void ChangeDepthBack(void);
extern void DumpBackgrounds(void);

int main(int argc, char* argv[])
{
    Initialise();
    
    InterfaceLoop();
    
    CleanUp(FALSE);
    
    return 0;
}





void Initialise(void)
{
    SInt32 randomSeed;
    
    GetDateTime(&randomSeed);
    SetQDGlobalsRandomSeed(randomSeed);
    
    g = (GlobalStuff *)NewPtr(sizeof(GlobalStuff));
    if (!g) ExitToShell();
    
    g->mainBundle = CFBundleGetMainBundle();
    
    LoadPreferences();
    
    CheckDepth();
    
    g->inGame = 0;
    
    SetRect(&g->swapBounds, 0, 0, 620, 420);
    g->fireBounds = g->swapBounds;
    InsetRect(&g->fireBounds, 1, 1);		// yes, this _is_ right.
    
    g->fireSwap = 0;
    EmptyFireBuffer();
    
    g->baseScoreEffect = NULL;
    g->baseShotEffect = NULL;
    g->baseSheep = NULL;
    g->baseWeapon = NULL;
    g->baseLevel = NULL;
    g->baseSceneryType = NULL;
    
    g->theWeapon = NULL;
    g->theLevel = NULL;
    
    LoadGlobalGraphics();
    LoadEveryThingElse();
    
    LoadInterface();	// that is, windows and meubars.
    
    g->windowActive = 1;
    
    InstallEventHandlers();
    
    if ( g->theWindow != GetUserFocusWindow() )
        g->windowActive = 0;
}






void InterfaceLoop(void)
{
    DrawBorder();
    
    DrawGWorldToWindow(g->interfaceBackGWorld, g->theWindow);
    
    RunApplicationEventLoop();
}


void CleanUp(bool instaQuit)
{
    short i = 0;
    
    if (g->theWindow) DisposeWindow(g->theWindow);
    if (g->aboutWindow) DisposeWindow(g->aboutWindow);
    if (g->instructionsWindow) DisposeWindow(g->instructionsWindow);
    if (g->prefsWindow) DisposeWindow(g->prefsWindow);
    if (g->highScoreWindow) DisposeWindow(g->highScoreWindow);
    if (g->gameOverWindow) DisposeWindow(g->gameOverWindow);
    if (g->highScoresWindow) DisposeWindow(g->highScoresWindow);
    if (g->levelSelectWindow) DisposeWindow(g->levelSelectWindow);
    if (g->autoPauseWindow) DisposeWindow(g->autoPauseWindow);
    if (g->askSwitchWindow) DisposeWindow(g->askSwitchWindow);
    
    if (g->theTimer) RemoveEventLoopTimer(g->theTimer);
    
    if (g->swapGWorld) DisposeGWorld(g->swapGWorld);
    if (g->interfaceBackGWorld) DisposeGWorld(g->interfaceBackGWorld);
    if (g->interfaceButtons) DisposeGWorld(g->interfaceButtons);
    if (g->interfaceRollover) DisposeGWorld(g->interfaceRollover);
    if (g->interfaceClick) DisposeGWorld(g->interfaceClick);
    if (g->aboutGWorld) DisposeGWorld(g->aboutGWorld);
    if (g->instructionsGWorld) DisposeGWorld(g->instructionsGWorld);
    if (g->voteGWorld) DisposeGWorld(g->voteGWorld);
    
    if (g->gunsPictGWorld) DisposeGWorld(g->gunsPictGWorld);
    if (g->gunsPictMask) DisposeGWorld(g->gunsPictMask);
    if (g->lifePictGWorld) DisposeGWorld(g->lifePictGWorld);
    if (g->lifePictMask) DisposeGWorld(g->lifePictMask);
    if (g->sheepArrowGWorld) DisposeGWorld(g->sheepArrowGWorld);
    if (g->sheepArrowMask) DisposeGWorld(g->sheepArrowMask);
    
    if (g->SCBonusGWorld) DisposeGWorld(g->SCBonusGWorld);
    if (g->SKBonusGWorld) DisposeGWorld(g->SKBonusGWorld);
    if (g->LCBonusGWorld) DisposeGWorld(g->LCBonusGWorld);
    if (g->HFBonusGWorld) DisposeGWorld(g->HFBonusGWorld);
    if (g->JDBonusGWorld) DisposeGWorld(g->JDBonusGWorld);
    if (g->CUBonusGWorld) DisposeGWorld(g->CUBonusGWorld);
    if (g->bonusMask) DisposeGWorld(g->bonusMask);
    if (g->chainBonusGWorld) DisposeGWorld(g->chainBonusGWorld);
    if (g->chainBonusMask) DisposeGWorld(g->chainBonusMask);
    while (i < 5)
    {
        if (g->extraBonusGWorld[i]) DisposeGWorld(g->extraBonusGWorld[i]);
        i++;
    }
    
    if (g->msgLevelStartGWorld) DisposeGWorld(g->msgLevelStartGWorld);
    if (g->msgLevelStartMask) DisposeGWorld(g->msgLevelStartMask);
    if (g->msgLevelEndGWorld) DisposeGWorld(g->msgLevelEndGWorld);
    if (g->msgLevelEndMask) DisposeGWorld(g->msgLevelEndMask);
    if (g->msgGameOverGWorld) DisposeGWorld(g->msgGameOverGWorld);
    if (g->msgGameOverMask) DisposeGWorld(g->msgGameOverMask);
    if (g->msgCompletedGWorld) DisposeGWorld(g->msgCompletedGWorld);
    if (g->msgCompletedMask) DisposeGWorld(g->msgCompletedMask);
    if (g->msgPausedGWorld) DisposeGWorld(g->msgPausedGWorld);
    if (g->msgPausedMask) DisposeGWorld(g->msgPausedMask);
    
    if (g->theSheepType.liveSpriteRunRightA) DisposeGWorld(g->theSheepType.liveSpriteRunRightA);
    if (g->theSheepType.liveSpriteRunRightMaskA) DisposeGWorld(g->theSheepType.liveSpriteRunRightMaskA);
    if (g->theSheepType.liveSpriteRunRightB) DisposeGWorld(g->theSheepType.liveSpriteRunRightB);
    if (g->theSheepType.liveSpriteRunRightMaskB) DisposeGWorld(g->theSheepType.liveSpriteRunRightMaskB);
        
    if (g->theSheepType.liveSpriteRunLeftA) DisposeGWorld(g->theSheepType.liveSpriteRunLeftA);
    if (g->theSheepType.liveSpriteRunLeftMaskA) DisposeGWorld(g->theSheepType.liveSpriteRunLeftMaskA);
    if (g->theSheepType.liveSpriteRunLeftB) DisposeGWorld(g->theSheepType.liveSpriteRunLeftB);
    if (g->theSheepType.liveSpriteRunLeftMaskB) DisposeGWorld(g->theSheepType.liveSpriteRunLeftMaskB);
    
    if (g->theSheepType.originalDeadSpriteRight) DisposeGWorld(g->theSheepType.originalDeadSpriteRight);
    if (g->theSheepType.originalDeadSpriteRightMaskWithoutOutline) DisposeGWorld(g->theSheepType.originalDeadSpriteRightMaskWithoutOutline);
    if (g->theSheepType.originalDeadSpriteRightMaskWithOutline) DisposeGWorld(g->theSheepType.originalDeadSpriteRightMaskWithOutline);
    
    if (g->theSheepType.originalDeadSpriteLeft) DisposeGWorld(g->theSheepType.originalDeadSpriteLeft);
    if (g->theSheepType.originalDeadSpriteLeftMaskWithoutOutline) DisposeGWorld(g->theSheepType.originalDeadSpriteLeftMaskWithoutOutline);
    if (g->theSheepType.originalDeadSpriteLeftMaskWithOutline) DisposeGWorld(g->theSheepType.originalDeadSpriteLeftMaskWithOutline);
    
    if (g->theScoreStuff.scoreGraphics) DisposeGWorld(g->theScoreStuff.scoreGraphics);
    if (g->theScoreStuff.multiplierGraphics) DisposeGWorld(g->theScoreStuff.multiplierGraphics);
    if (g->theScoreStuff.scoreMask) DisposeGWorld(g->theScoreStuff.scoreMask);
    
    ReleaseResource((Handle)g->crosshair);
    
    FinishSounds();
    
    RemoveAllSheep();
    RemoveAllScoreEffects();
    RemoveAllShotEffects();
    
    RemoveAllWeapons();
    RemoveAllLevels();
    RemoveAllScenery();
    DumpBackgrounds(); // why not RemoveAll…? search me.
    
    ChangeDepthBack();
    
    UnloadHighScores(); // Incidentally, writes prefs&highscores to disk
    
    if (g) DisposePtr((Ptr)g);
    
    if (instaQuit)
        ExitToShell();
}

