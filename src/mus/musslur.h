/////////////////////////////////////////////////////////////////////////////
// Name:        musslur.h
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_SLUR_H__
#define __MUS_SLUR_H__

#include "muslayer.h"
#include "musnote.h"
#include "musobject.h"

//----------------------------------------------------------------------------
// MusSlur
//----------------------------------------------------------------------------

class MusSlur: public MusLayerElement
{
public:
    // constructors and destructors
    MusSlur();
    virtual ~MusSlur();
    
    virtual std::string MusClassName( ) { return "MusSlur"; };
    
	int GetNoteCount() const { return (int)m_children.size(); };
    
    /**
     * Add an element (a note or a rest) to a beam.
     * Only MusNote or MusRest elements will be actually added to the beam.
     */
    void AddNote(MusLayerElement *element);
    
private:
    
public:
    
private:
    
};

#endif
