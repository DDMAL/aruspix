//
//  muskeysig.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 10/07/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef aruspix_muskeysig_h
#define aruspix_muskeysig_h

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "muslayer.h"
#include "muspositioninterface.h"
#include "musdef.h"


class MusKeySig: public MusLayerElement, public MusPositionInterface
{
public:
    // constructors and destructors
    MusKeySig();
    MusKeySig(int num_alter, char alter);
    virtual ~MusKeySig();
    
    virtual wxString MusClassName( ) { return "MusKeySig"; };
    
    unsigned char GetAlterationAt(int pos);
    int GetOctave(unsigned char pitch);
    
private:
    
public:
    int m_num_alter;
    unsigned char m_alteration;
    
private:
    static unsigned char flats[];
    static unsigned char sharps[];
    static unsigned int octave_map[][7];
    
};


#endif
