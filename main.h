/*
 *  main.h
 *  nibMAFF
 *
 *  Created by wibble on Wed Sep 04 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */


#include <Carbon/Carbon.h>

#include "mafftypes.h"

#include "loading.h"
#include "graphics.h"
#include "eventhandlers.h"
#include "game.h"
#include "scores.h"


void Initialise(void);

void InterfaceLoop(void);

void CleanUp(bool instaQuit);


GlobalStuff		*g;

