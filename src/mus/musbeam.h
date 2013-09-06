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

#include "muslayerelement.h"
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
    
	int GetNoteCount() const { return (int)m_children.GetCount(); };
    
    /**
     * Add an element (a note or a rest) to a beam.
     * Only MusNote or MusRest elements will be actually added to the beam.
     */
    void AddNote(MusLayerElement *element);
    
    // functor
    //virtual bool Save( wxArrayPtrVoid params );
    
private:

public:
    
private:
    
};

#endif
