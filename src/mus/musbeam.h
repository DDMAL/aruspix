//
//  musbeam.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef __MUS_BEAM_H__
#define __MUS_BEAM_H__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "musobject.h"
#include "muslayer.h"
#include "musnote.h"

//----------------------------------------------------------------------------
// MusBeam
//----------------------------------------------------------------------------

class MusBeam: public MusLayerElement
{
public:
    // constructors and destructors
    MusBeam();
    virtual ~MusBeam();
    
    virtual wxString MusClassName( ) { return "MusBeam"; };
    
    void AddNote(MusNote *note);
    
private:
    ArrayOfMusLayerElements m_notes;
    
public:
    
private:
    
};

#endif
