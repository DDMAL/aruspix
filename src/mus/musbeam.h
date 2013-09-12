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

class MusBeam: public MusLayerElement, public MusObjectListInterface
{
public:
    // constructors and destructors
    MusBeam();
    virtual ~MusBeam();
    
    virtual wxString MusClassName( ) { return "MusBeam"; };
    
    int GetNoteCount() const { return (int)m_children.size(); };
    
    //int GetNoteCount();
    
    
    
    /**
     * Add an element (a note or a rest) to a beam.
     * Only MusNote or MusRest elements will be actually added to the beam.
     */
    void AddElement(MusLayerElement *element);
    
    // functor
    //virtual bool Save( wxArrayPtrVoid params );
    
protected:
    /**
     * Filter the list for a specific class.
     * For example, keep only notes in MusBeam
     */
    virtual void FilterList();
    
private:

public:
    
private:
    
};

#endif
