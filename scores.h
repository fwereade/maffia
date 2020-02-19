/*
 *  scores.h
 *  nibMAFF
 *
 *  Created by William Reade on Thu Oct 17 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "mafftypes.h"

void LoadHighScores(void);

void SaveHighScores(void);

void DefaultHighScores(void);

bool NewHighScore(void);

void UpdateHighScores(void);

void UnloadHighScores(void);


