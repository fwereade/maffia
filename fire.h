/*
 *  fire.h
 *  nibMAFF
 *
 *  Created by wibble on Tue Sep 17 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

#define FIRE_COLOUR_HI (unsigned char)124
#define FIRE_COLOUR_LO (unsigned char)0

#define SMOKE_COLOUR_HI (unsigned char)0
#define SMOKE_COLOUR_LO (unsigned char)64

void		ProcessAndDrawFire			(GWorldPtr theGWorld);

