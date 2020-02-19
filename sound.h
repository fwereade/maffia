/*
 *  sound.h
 *  nibMAFF
 *
 *  Created by William Reade on Sat Oct 26 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "mafftypes.h"

void LoadSounds(void);
void FinishSounds(void);

void PlaySound(short channel, Handle sound);