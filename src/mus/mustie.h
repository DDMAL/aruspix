//
//  musbeam.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef __MUS_TIE_H__
#define __MUS_TIE_H__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "musobject.h"
#include "muslayer.h"
#include "musnote.h"

//----------------------------------------------------------------------------
// MusTie
// A TIE class is a simple slur - it takes in account only two notes, the first and second:
// NOTE tie NOTE tie etc
//----------------------------------------------------------------------------

class MusTie: public MusLayerElement
{
public:
    // constructors and destructors
    MusTie();
    virtual ~MusTie();
    
    virtual wxString MusClassName( ) { return "MusTie"; };
    
    MusNote *m_first;
    MusNote *m_second;
    
private:
    
public:
    
private:
    
};

#endif
