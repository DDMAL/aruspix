
/////////////////////////////////////////////////////////////////////////////
// Name:        muskeysig.h
// Author:      Rodolfo Zitellini
// Created:     10/07/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_KEYSIG_H__
#define __MUS_KEYSIG_H__

#include "musdef.h"
#include "muslayerelement.h"
#include "muspositioninterface.h"


//----------------------------------------------------------------------------
// MusKeysig
//----------------------------------------------------------------------------

class MusKeySig: public LayerElement, public PositionInterface
{
public:
    // constructors and destructors
    MusKeySig();
    MusKeySig(int num_alter, char alter);
    virtual ~MusKeySig();
    
    virtual std::string MusClassName( ) { return "MusKeySig"; };
    
    /**
     * Return the default horizontal spacing after a keysig.
     */
    virtual int GetHorizontalSpacing( ) { return 20; };
    
    unsigned char GetAlterationAt(int pos);
    int GetOctave(unsigned char pitch, char clef);
    
private:
    
public:
    int m_num_alter;
    unsigned char m_alteration;
    
private:
    static unsigned char flats[];
    static unsigned char sharps[];
    static int octave_map[2][9][7];
    
};


#endif
