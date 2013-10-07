/////////////////////////////////////////////////////////////////////////////
// Name:        mustie.h
// Author:      Rodolfo Zitellini
// Created:     26/06/2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_TIE_H__
#define __MUS_TIE_H__

#include "muslayer.h"
#include "musnote.h"
#include "musobject.h"

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
    
    virtual std::string MusClassName( ) { return "MusTie"; };
    
    MusNote *m_first;
    MusNote *m_second;
    
private:
    
public:
    
private:
    
};

#endif
