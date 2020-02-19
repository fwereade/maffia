/*
 *  mafftypes.h
 *  nibMAFF
 *
 *  Created by wibble on Fri Sep 06 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

#ifndef __MAFFTYPES__

#define __MAFFTYPES__ 1

enum
{
	kNothing = 0,
	kNewGame,
	kHighScores,
	kPreferences,
	kQuit
};

enum
{
	kUzi = 1,
	kMagnum,
	kShotgun,
	kFlamer
};

enum				// g->gameState
{
        kLevelStart,
        kLevelEnd,
        kPlaying,		// hidden dependency: states <= kPlaying allow weapon selection.
        kGameOverScreen,
        kCompletedScreen
};

enum
{
    kNoBonus,
    kLambChopBonus,
    kSwissCheeseBonus,
    kShishKebabBonus,
    kHighFlierBonus,
    kJammyDodgerBonus,
    kChainUpBonus
};

enum
{
    kHitNowt,
    kHitScenery,
    kHitSheep
};


#define kGravity 0.25
#define kSheepAnimWait 1
#define LIVES_START 3

#define NUM_ARROWS 32

#define BONUS_TIME_LENGTH 2.5

#define BORDER_WIDTH 30
#define BORDER_HEIGHT 30

#define NUM_SOUND_CHANNELS 6

enum
{
    kShotChannel = 0,
    kSplatChannel,
    kBaaChannel,
    kBonusChannel,
    kChainChannel,
    kBleepChannel
};

#define kOn 1
#define kOff 0

typedef struct pointFloat
{
	float		x;
	float		y;
} pointFloat;





typedef struct Weapon
{
        short		weaponID;
        GWorldPtr	holeSprite;
        GWorldPtr	holeSpriteMaskWithoutOutline;
        GWorldPtr	holeSpriteMaskWithOutline;
        GWorldPtr	holeSpriteDrawMask;
        Rect		holeRect;
        GWorldPtr	holeTempMask;
        EventTime	reloadTime;
        float		recoil;
        float		scoreMultiplier;
	EventTime	lastFired;
        
        Handle		shotSoundHandle;
        
	struct Weapon	*next;
        
} Weapon;


typedef struct SheepType
{
        GWorldPtr	liveSpriteRunRightA;
        GWorldPtr	liveSpriteRunRightMaskA;
        GWorldPtr	liveSpriteRunRightB;
        GWorldPtr	liveSpriteRunRightMaskB;
        
        GWorldPtr	liveSpriteRunLeftA;
        GWorldPtr	liveSpriteRunLeftMaskA;
        GWorldPtr	liveSpriteRunLeftB;
        GWorldPtr	liveSpriteRunLeftMaskB;
        
        GWorldPtr	originalDeadSpriteRight;
        GWorldPtr	originalDeadSpriteRightMaskWithoutOutline;
        GWorldPtr	originalDeadSpriteRightMaskWithOutline;
        GWorldPtr	originalDeadSpriteLeft;
        GWorldPtr	originalDeadSpriteLeftMaskWithoutOutline;
        GWorldPtr	originalDeadSpriteLeftMaskWithOutline;
        
        GWorldPtr	splitMask;
        
        Rect		deadBounds;
	
} SheepType;


typedef struct SheepToken
{
        Boolean			readyToDie;
        short			layer;
        pointFloat		position;
        pointFloat		velocity;
        Rect			deadBounds;
        GWorldPtr		deadSprite;
        GWorldPtr		deadSpriteMaskWithoutOutline;
        GWorldPtr		deadSpriteMaskWithOutline;
        GWorldPtr		burnMask;
        unsigned long		timesShot;
        Boolean			isBurning;
        
        short			frame;
        EventTime		lastFrameTime;
        
	struct SheepToken	*next;
	struct SheepToken	*prev;
        
} SheepToken;

typedef struct StartPoint
{
        short			posY;
        short			layer;
	struct StartPoint	*next;
        
} StartPoint;

typedef struct SceneryType
{
        CFStringRef		sceneryName;
        GWorldPtr		spriteGWorld;
        GWorldPtr		maskGWorld;
        Rect			bounds;
	struct SceneryType	*next;
	
} SceneryType;

typedef struct Background
{
        CFStringRef		name;
        GWorldPtr		theGWorld;
        struct Background	*next;
        
} Background;

typedef struct SceneryToken
{
        short			layer;
	SceneryType		*type;
	pointFloat		position;
	
	struct SceneryToken	*next;
	
} SceneryToken;

typedef struct Level
{
        short			levelNumber;
        
        //GWorldPtr		background;
        Background		*theBackground;
        
	SceneryToken		*baseSceneryToken;
        
        unsigned long		numSheep;
        EventTime		minDelay;
        EventTime		maxDelay;
        float			minSpeed;
        float			maxSpeed;
        
	StartPoint		*baseStartPoint;
        short			numStartPoints;
				
	struct Level		*next;
	
} Level;

typedef struct ScoreStuff
{
        GWorldPtr	scoreGraphics;
        GWorldPtr	scoreMask;
        GWorldPtr	multiplierGraphics;
        Rect		numberRect;
	unsigned long	score;
        unsigned long	multiplier;
        unsigned long	longestJuggle;
        unsigned long	highestMultiplier;
        unsigned long	sheepKilledTotal;
        unsigned long	sheepUsedThisLevel;
        short		livesLeft;
        short		livesLeftBeeps;
				
} ScoreStuff;
				
typedef struct HighScoreList
{
        unsigned long	score[10];
        CFStringRef	scoreName[10];
        short		lastScoreIndex;
        
        unsigned long	sheepKilled[10];
        CFStringRef	killedName[10];
        short		lastKilledIndex;
        
        unsigned long	highestMultiplier[10];
        CFStringRef	multName[10];
        short		lastMultIndex;
        
        unsigned long	longestJuggle[10];
        CFStringRef	juggleName[10];
        short		lastJuggleIndex;
        
        CFStringRef	lastName;
        short		lastLookedAt;
        
} HighScoreList;


typedef struct ScoreEffect
{
    Boolean		readyToDie;
    short		score;
    short		multiplier;
    short		totalCharacters;
    short		scoreCharacters;
    short		multiplierCharacters;
    short		animationFrame;
    Point		position;
    struct ScoreEffect	*next;
    
} ScoreEffect;

typedef struct ShotEffect
{
    Boolean		readyToDie;
    short		type;
    Point		position;
    short		animationFrame;
    struct ShotEffect	*next;
    
} ShotEffect;

typedef struct PreferencesStruct
{
    Boolean		blood;
    Boolean		fire;
    Boolean		sound;
    Boolean		turbo;
    Boolean		shotFX;
    Boolean		border;
    Boolean		switchDepth;
} PreferencesStruct;

typedef struct SaveData
{
    PreferencesStruct	savePrefs;
    HighScoreList	saveScores;
} SaveData;

typedef struct GlobalSounds
{
    Handle		SCBonus;	// done
    Handle		LCBonus;	// done
    
    Handle		splat;		// done
    
    Handle		baa;
    
    Handle		weaponSwitch;	// done
    Handle		stateSwitch;	// done?
    Handle		lastLife;	// done
    
    Handle		clank;
    Handle		breakchain;	////////////// all done ;)
    
} GlobalSounds;


typedef struct GlobalStuff
{
    bool		firstRunDoAbout;
    bool		askSwitchDepthPreferences;
    short		startNewGameImmediately;
    
    short		wasDepth;
    bool		depthChanged;
    
    bool		inGame;
    short		gameState;
    bool		windowActive;
    bool		alreadyAutoPaused;
    
    PreferencesStruct	pref;
    HighScoreList	scores;
    
    CFBundleRef		mainBundle;
    
    WindowRef		theWindow;
    WindowRef		aboutWindow;
    WindowRef		instructionsWindow;
    WindowRef		prefsWindow;
    WindowRef		highScoreWindow;
    WindowRef		gameOverWindow;
    WindowRef		highScoresWindow;
    WindowRef		levelSelectWindow;
    WindowRef		autoPauseWindow;
    WindowRef		askSwitchWindow;
    
    EventLoopTimerRef	theTimer;
    float		fps;
    
    EventTime		lastAboutStyleWindowTime;
    
    EventTime		frameTime;
    EventTime		lastStateSwitchTime;
    
    EventTime		lastBonusTime;
    short		bonusChainLength;
    short		lastBonus;
    UInt32		lastBonusData;
    bool		lastBonusDataLife;
    bool		bonusAlreadyThisFrame;	// only affects sound, not anything else
    
    SndChannelPtr	sndChannel[NUM_SOUND_CHANNELS];
    
    GlobalSounds	sounds;
    
    CursHandle		crosshair;
    
    Rect		swapBounds;		// 0,0 bounds for main window
    Rect		fireBounds;		// swapbounds inset by 1 for calc purposes
    
    GWorldPtr		swapGWorld;		// back buffer
    
    bool		fireSwap;
    unsigned char	fireArray[260400];	// 620 * 420
    
    SInt16		sheepArrows[NUM_ARROWS];
    SInt16		sheepArrowsThisFrame;
    
    SheepType		theSheepType;
    ScoreStuff		theScoreStuff;
    
    ScoreEffect		*baseScoreEffect;
    ShotEffect		*baseShotEffect;
    SheepToken		*baseSheep;
    Weapon		*baseWeapon;
    Level		*baseLevel;
    SceneryType		*baseSceneryType;
    Background		*baseBackground;
    
    Weapon		*theWeapon;
    Level		*theLevel;
    
    GWorldPtr		interfaceBackGWorld;
    GWorldPtr		interfaceButtons;
    GWorldPtr		interfaceRollover;
    GWorldPtr		interfaceClick;
    GWorldPtr		aboutGWorld;
    GWorldPtr		instructionsGWorld;
    GWorldPtr		voteGWorld;
    
    GWorldPtr		gunsPictGWorld;
    GWorldPtr		gunsPictMask;
    GWorldPtr		lifePictGWorld;
    GWorldPtr		lifePictMask;
    GWorldPtr		sheepArrowGWorld;
    GWorldPtr		sheepArrowMask;
    
    GWorldPtr		SCBonusGWorld;
    GWorldPtr		SKBonusGWorld;
    GWorldPtr		LCBonusGWorld;
    GWorldPtr		HFBonusGWorld;
    GWorldPtr		JDBonusGWorld;
    GWorldPtr		CUBonusGWorld;
    GWorldPtr		bonusMask;
    GWorldPtr		chainBonusGWorld;
    GWorldPtr		chainBonusMask;
    GWorldPtr		extraBonusGWorld[5];
    
    GWorldPtr		msgLevelStartGWorld;
    GWorldPtr		msgLevelStartMask;
    GWorldPtr		msgLevelEndGWorld;
    GWorldPtr		msgLevelEndMask;
    GWorldPtr		msgGameOverGWorld;
    GWorldPtr		msgGameOverMask;
    GWorldPtr		msgCompletedGWorld;
    GWorldPtr		msgCompletedMask;
    GWorldPtr		msgPausedGWorld;
    GWorldPtr		msgPausedMask;
    
    short		mouseOverOption;
    short		clickedOnOption;
    
} GlobalStuff;

#endif