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

class Tuplet: public MusLayerElement, public MusObjectListInterface
{
public:
    // constructors and destructors
    Tuplet();
    virtual ~Tuplet();
    
    virtual std::string MusClassName( ) { return "Tuplet"; };

	int GetNoteCount() const { return (int)m_children.size(); };
    
    /**
     * Add an element (a note or a rest) to a tuplet.
     * Only MusNote or Rest elements will be actually added to the beam.
     */
    void AddElement(MusLayerElement *element);
    
protected:
    /**
     * Filter the list for a specific class.
     * For example, keep only notes in MusBeam
     */
    virtual void FilterList();
    
private:
    
public:
    int m_num;
    int m_numbase;
    
private:

};

#endif
