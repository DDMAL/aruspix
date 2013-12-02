/////////////////////////////////////////////////////////////////////////////
// Name:        mustuplet.h
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_TUPLET_H__
#define __MUS_TUPLET_H__

#include "muslayer.h"
#include "musnote.h"
#include "musobject.h"

//----------------------------------------------------------------------------
// MusBeam
//----------------------------------------------------------------------------

class MusTuplet: public MusLayerElement
{
public:
    // constructors and destructors
    MusTuplet();
    virtual ~MusTuplet();
    
    virtual std::string MusClassName( ) { return "MusTuplet"; };
    virtual std::string GetIdShortName() { return "tuplet-"; };
    
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
