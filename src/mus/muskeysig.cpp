//
//  muskeysig.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 10/07/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "muskeysig.h"


//----------------------------------------------------------------------------
// MusKeySig
//----------------------------------------------------------------------------

unsigned char MusKeySig::flats[] = {PITCH_B, PITCH_E, PITCH_A, PITCH_D, PITCH_G, PITCH_C, PITCH_F};
unsigned char MusKeySig::sharps[] = {PITCH_F, PITCH_C, PITCH_G, PITCH_D, PITCH_A, PITCH_E, PITCH_B};

unsigned int MusKeySig::octave_map[][7] = {
   //C, D, E, F, G, A, B 
    {1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 0},
};

MusKeySig::MusKeySig():
MusLayerElement(), MusPositionInterface()
{
    MusKeySig(0, ACCID_NATURAL);
}

MusKeySig::MusKeySig(int num_alter, char alter):
MusLayerElement(), MusPositionInterface()
{
    m_num_alter = num_alter;
    m_alteration = alter;
}

MusKeySig::~MusKeySig()
{
}

unsigned char MusKeySig::GetAlterationAt(int pos) {
    unsigned char *alteration_set;
    
    if (pos > 6)
        return 0;
    
    if (m_alteration == ACCID_FLAT)
        alteration_set = flats;
    else
        alteration_set = sharps;
    
    return alteration_set[pos];
}

int MusKeySig::GetOctave(unsigned char pitch) {
    return octave_map[0][pitch - 1];
}