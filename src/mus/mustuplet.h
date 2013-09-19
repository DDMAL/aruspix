//
//  mustuplet.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 26/06/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef __MUS_TUPLET_H__
#define __MUS_TUPLET_H__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "musobject.h"
#include "muslayer.h"
#include "musnote.h"

//----------------------------------------------------------------------------
// MusBeam
//----------------------------------------------------------------------------

class MusTuplet: public MusLayerElement
{
public:
    // constructors and destructors
    MusTuplet();
    virtual ~MusTuplet();
    
    virtual wxString MusClassName( ) { return "MusTuplet"; };
    
	int GetNoteCount() const { return (int)m_children.size(); };
    
    /**
     * Add an element (a note or a rest) to a tuplet.
     * Only MusNote or MusRest elements will be actually added to the beam.
     */
    void AddElement(MusLayerElement *element);
    
private:
    
public:
    
private:
    
};

#endif
