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
    
    /**
     * Add an element (a note or a rest) to a beam.
     * Only MusNote or MusRest element will be actually added to the beam.
     */
    void AddNote(MusLayerElement *element);
    
private:
    /**
     * The array of notes or rests.
     * The beam object do not own the notes.
     */
    ArrayOfMusLayerElements m_notes;
    
public:
    
private:
    
};

#endif
