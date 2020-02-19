/*
 *  depthcheck.c
 *  MAFFia
 *
 *  Created by William Reade on Tue Nov 05 2002.
 *  Copyright (c) 2002 William Reade. All rights reserved.
 *
 */

#include "depthcheck.h"

#include "mafftypes.h"

#define PRF_CUR kCFPreferencesCurrentApplication

extern GlobalStuff *g;

void CheckDepth(void)
{
    GDHandle	theGDevice;
    Boolean	valid;
    
    g->depthChanged = FALSE;
    g->askSwitchDepthPreferences = FALSE;
    
    theGDevice = GetMainDevice();
    
    g->wasDepth = (**(**theGDevice).gdPMap).pixelSize;
    
    if (g->wasDepth != 16)
    {
        g->pref.switchDepth = CFPreferencesGetAppBooleanValue(CFSTR("switchDepth"), PRF_CUR, &valid);
        
        if (valid)
        {
            if (g->pref.switchDepth)
            {
                ChangeDepth();
            }
        }
        else
            g->askSwitchDepthPreferences = TRUE;
    }
}


void ChangeDepth(void)
{
    GDHandle		theGDevice;
    
    theGDevice = GetMainDevice();
    
    if ( HasDepth(theGDevice, 16, 1, 1) )
    {
        SetDepth(theGDevice, 16, 1, 1);
        g->depthChanged = TRUE;
    }
}

void ChangeDepthBack(void)
{
    GDHandle	theGDevice;
    
    if (g->depthChanged)
    {
        theGDevice = GetMainDevice();
        SetDepth(theGDevice, g->wasDepth, 1, 1);
    }
}

