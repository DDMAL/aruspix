/////////////////////////////////////////////////////////////////////////////
// Name:        musbeam.h
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//

#ifndef __MUS_BEAM_H__
#define __MUS_BEAM_H__

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
    
    virtual std::string MusClassName( ) { return "MusBeam"; };
    virtual std::string GetIdShortName() { return "beam-"; };
    
    int GetNoteCount() const { return (int)m_children.size(); };
    
    //int GetNoteCount();
    
    
    
    /**
     * Add an element (a note or a rest) to a beam.
     * Only MusNote or MusRest elements will be actually added to the beam.
     */
    void AddElement(MusLayerElement *element);
    
    // functor
    //virtual int Save( ArrayPtrVoid params );
    
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
